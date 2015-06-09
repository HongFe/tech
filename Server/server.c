#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <json/json.h>
#include <my_global.h>
#include <stdio.h>
#include <string.h>
#include <mysql.h>

#pragma comment(lib, "libmySQL.lib")

#include "server.h"

int main()
{
	init_socket();						// 소켓 초기화
	connect_mysql();					// mysql 연결
	
	pthread_create(&p_thread, NULL, process_accept, NULL);
	pthread_join(p_thread,(void **)&status);

	close(server_socket);
	mysql_free_result(res);
	mysql_close(conn_ptr);

}

///////////////////////////////////////////////////
// \brief	mysql에 연결
// \section update
//	- 2015-01-23
///////////////////////////////////////////////////
int connect_mysql()
{
	conn_ptr = mysql_init(NULL);
	if (!conn_ptr)
	{
		printf("mysql_init failed!\n");
		return 1;
	}

	//MySQL connect
	conn_ptr = mysql_real_connect(conn_ptr, HOST, USER, PASS, NAME, 3306, (char*)NULL, 0);
	if (conn_ptr)
		printf("Connect success!\n");
	else{
		printf("Connect fail!\n");
		return 1;
	}

	return 0;
}

///////////////////////////////////////////////////
// \brief	소켓 초기화
// \section update
//	- 2015-01-23
///////////////////////////////////////////////////
int init_socket()
{
	server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (-1 == server_socket)
	{
		printf("creating serverIME_STR) socket is failed\n");
		return 1;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4000);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (-1 == bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
	{
		printf("bind error\n");
		return 1;
	}

	if (-1 == listen(server_socket, 5))
	{
		printf("listen error\n");
		return 1;
	}

	return 0;
}

///////////////////////////////////////////////////
// \brief	접속요청 받음
// \param	소켓 임시 변수
// \section update
//	- 2015-01-23
///////////////////////////////////////////////////
void *process_accept()
{
	struct sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);
	int *socketPtr;

	while(1)
	{
		socketPtr = malloc(sizeof(int));
		*socketPtr = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size); // 클라이언트와 연결된 소켓을 저장해놓고

		printf("connected %d\n", *socketPtr);
		if (-1 == *socketPtr)
		{
			printf("accept error\n");
			pthread_exit(0);
		}
		pthread_create(&g_thread, NULL, process_recieve, (void *)socketPtr);
		pthread_detach(g_thread);	
	}
	pthread_exit(0);
}

///////////////////////////////////////////////////
// \brief	json 데이터 받고 처리
// \param	소켓 번호
// \section update
//	- 2015-01-23
///////////////////////////////////////////////////
void *process_recieve(void *arg)
{
	int *socketNumber = (int *)arg;
	int i = 0;
	int nRead = 0;
	char buf[1024];
	int packetSize=0;

	printf("%d\n", *socketNumber);


	while (0 < (nRead = read(*socketNumber, &packetSize, 4)))
	{	
		printf("%d\n",packetSize);
		if(MAX_PACKETSIZE < packetSize)
			continue;
		if( 0 >= read(*socketNumber, buf, packetSize) )
			break;
		
		printf("--------------------------------------\n");
		printf("%s from %d\n", buf, *socketNumber);
		printf("--------------------------------------\n");
			

		JParsing(buf);
	}
	close(*socketNumber);
	free(socketNumber);
	pthread_exit(0);
}

///////////////////////////////////////////////////
// \brief	json 데이터를 받아 파싱해 db에 삽입
// \param	json 데이터가 저장되어있는 버퍼
// \section update
//	- 2015-01-23
///////////////////////////////////////////////////
int JParsing(char *buff)
{
	json_object *my_obj, *sentinel_obj;
	json_object *sentinel_str, *sentinel_val, *passengerInfo_array, *passengerInfo_obj, *passengerInfo_val;
	int i;

	my_obj = json_tokener_parse(buff); //myobj : buff 정보 들어감
	if (NULL == my_obj) {
		printf("Wrong Format!\n");
 		return 1; // oops, we failed. 
	}

	//T_Sentinel에서 object 추출
	sentinel_obj = json_object_object_get(my_obj, "T_Sentinel");
	if (NULL == sentinel_obj) {
		printf("Wrong Format!\n");
 		return 1; // oops, we failed. 
	}

	//T_Sentinel 파싱
	sentinel_str = json_object_object_get(sentinel_obj, "SentinelID");
	if (NULL == sentinel_str) {
		printf("Wrong Format!\n");
 		return 1; // oops, we failed. 
	}
	//PassengerInfo에서 object 추출
	passengerInfo_array = json_object_object_get(sentinel_obj, "PassengerInfo");
	if (NULL == passengerInfo_array) {
		printf("Wrong Format!\n");
 		return 1; // oops, we failed. 
	}
	//PassengerTotalNum번 for문 돌아감
	for (i = 0; i < json_object_array_length(passengerInfo_array); i++)
	{
		passengerInfo DBform;

		passengerInfo_obj = json_object_array_get_idx(passengerInfo_array, i);
		passengerInfo_val = json_object_object_get(passengerInfo_obj, "PassengerID");
		if (NULL == passengerInfo_val) {
			printf("Wrong Format!\n");
	 		return 1; // oops, we failed. 
	}
		DBform.passengerid = json_object_get_string(passengerInfo_val);	
		passengerInfo_val = json_object_object_get(passengerInfo_obj, "BatteryLevel");		//passengerid
		if (NULL == passengerInfo_val ) {
			printf("Wrong Format!\n");
	 		return 1; // oops, we failed. 
		}

		DBform.batterylevel = json_object_get_int(passengerInfo_val);				//BatteryLevel
		passengerInfo_val = json_object_object_get(passengerInfo_obj, "RSSI");
		if (NULL == passengerInfo_val) {
			printf("Wrong Format!\n");
	 		return 1; // oops, we failed. 
		}

		DBform.rssi = json_object_get_int(passengerInfo_val);					//rssi
		DBform.sentinelID = json_object_get_string(sentinel_str);				//sentinelID
		DBform.reportTime=TIME_STR;					
		DBform.illumination = 0;

		//MySQL query execute
		pthread_mutex_lock(&mutex);
		SQL_INSERT(DBform);
		pthread_mutex_unlock(&mutex);

		//printf("%s | %d | %d | %s | %s | %d\n", DBform.passengerid, DBform.batterylevel, DBform.rssi, DBform.sentinelID, DBform.reportTime, DBform.illumination);
	}
	return 0;
}

///////////////////////////////////////////////////
// \brief	insert query 실행
// \param	insert 할 데이터
// \section update
//	- 2015-01-23
///////////////////////////////////////////////////
void SQL_INSERT(passengerInfo DBform)
{
	 int count;

    char select_query[255] = "\0";
    char update_query[255] = "\0";
    char insert_query[255] = "\0";
    char history_query[255] = "\0";
    char delete_query[255] = "\0";
    char passengers_q[255] = "\0";

    sprintf(select_query, "select count(passengerid) as variable from passengerinfo where passengerid = '%s' and sentinelID = '%s';"
        , DBform.passengerid, DBform.sentinelID);
    sprintf(update_query, "update passengerinfo set reportTime = %s, batterylevel = '%d', rssi = '%d', illumination = '%d' where passengerid = '%s' and sentinelID = '%s';"
        , DBform.reportTime, DBform.batterylevel, DBform.rssi, DBform.illumination, DBform.passengerid, DBform.sentinelID);
    sprintf(insert_query, "insert into passengerinfo values('%s','%d','%d','%s', %s, '%d');"
        , DBform.passengerid, DBform.batterylevel, DBform.rssi, DBform.sentinelID, DBform.reportTime, DBform.illumination);
    sprintf(history_query, "insert into history(passengerid,batterylevel,rssi,sentinelID,reportTime,illumination) values('%s','%d','%d','%s', %s, %d);"
        , DBform.passengerid, DBform.batterylevel, DBform.rssi, DBform.sentinelID, DBform.reportTime, DBform.illumination);
    sprintf(delete_query, "delete from passengerinfo where passengerid = '%s' and \
        (time_to_sec(timediff(now(), reportTime)) >= 10 or illumination >= 30);", DBform.passengerid);
    sprintf(passengers_q, "update passengers set activation = \"1\" where pid = '%s';", DBform.passengerid); //////////////////

    //histroy 추가
    len = mysql_query(conn_ptr, history_query);

    //delete
    len = mysql_query(conn_ptr, delete_query);

    //update passengers table
    len = mysql_query(conn_ptr, passengers_q);
    
    //insert or update
    len = mysql_query(conn_ptr, select_query);
    res = mysql_store_result(conn_ptr);
    row = mysql_fetch_row(res);

    count = (int)strtol(row[0], (char**)NULL, 10); //select_query result

    if(1 <= count)
        len = mysql_query(conn_ptr, update_query);
    else
        len = mysql_query(conn_ptr, insert_query);
}

