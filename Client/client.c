#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#define  BUFF_SIZE   1024


void *chat(void *);

pthread_t thread;

struct data
{
  int strSize;
  char json[500];
};

int   main( int argc, char **argv)
{
   int   client_socket;
	char buf[100]= "abcdef";
	int nRead=0;
   struct sockaddr_in   server_addr;
   int count=0;
   int strSize=0;
   char   buff[BUFF_SIZE+5];
   struct  data dt;
   char Jstr[1024] = "{\"T_Sentinel\": {\"SentinelID\":\"SXXXXXX\",\"PassengerTotalNum\":3,\"PassengerInfo\": \
             [{\"PassengerID\":\"PXXXXX1\",\"BatteryLevel\":5,\"RSSI\":100}, \
             {\"PassengerID\":\"PXXXXY1\",\"BatteryLevel\":5,\"RSSI\":100}, \
             {\"PassengerID\":\"PXXXXZ1\",\"BatteryLevel\":5,\"RSSI\":100}]}}";


   client_socket  = socket( PF_INET, SOCK_STREAM, 0);
   if( -1 == client_socket)
   {
      printf( "socket 생성 실패\n");
      exit( 1);
   }

   system("clear");

   memset( &server_addr, 0, sizeof( server_addr));
   server_addr.sin_family     = AF_INET;
   server_addr.sin_port       = htons(4000);
   server_addr.sin_addr.s_addr= inet_addr( "127.0.0.1");

   if( -1 == connect( client_socket, (struct sockaddr*)&server_addr, sizeof( server_addr) ) )
   {
      printf( "접속 실패\n");
      exit( 1);
   }
	printf("socknumber : %d\n",client_socket);	
 	while(1)
	{	
	    usleep(500000);
	printf("%s",Jstr);
	    strSize = strlen(Jstr);
	    printf("strSIze : %d\n",strSize);
	    dt.strSize = strSize;
	    strcpy(dt.json,Jstr);
	    write(client_socket,&dt,strSize+4);
	    printf("   time = %d",(unsigned)time(NULL));
	    printf("  count =  %d\n",count++);
	}
   close(client_socket);
   return 0;
}

void *chat(void *arg)
{
	int sock = *(int *)arg; 
	int nRead=0;
	char buf[100];
	while(0 <  (nRead = read(sock,buf,100)) )
	{
		printf("nRead = %d\n sockNumber = %d",nRead,sock);
		printf("%s\n",buf);
	}
   close(sock);
   pthread_exit(0);
}
