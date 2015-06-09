/////////////////////////////////////////////////////////////////
///
/// \brief    센티널과 통신하고 받아온 자료를 DB에 저장하는 소스
/// 
/// \warning 
///
/// \version  1.0.0
///
/// \author   김세훈,김현준,나홍철,정성진
/// 
/// \date     2015/01/19
///
/// \section
/////////////////////////////////////////////////////////////////

#define MAX_CLIENT 5
#define MAX_PACKETSIZE 10000


//////// MYSQL///////////////////
#define HOST "localhost"
#define USER "root"
#define PASS "root"
#define NAME "tech_schem"
#define TIME_STR "now()"

int len = 0;
MYSQL* conn_ptr;
MYSQL_RES* res;
MYSQL_ROW row;

////////////////////////////////////
void **status;

int server_socket;

struct sockaddr_in server_addr;
struct sockaddr_in client_addr;

///////////////////////////


int g_numOfClients = 0;
int g_clientSocket[MAX_CLIENT];

pthread_t p_thread,g_thread;
pthread_mutex_t mutex;

// Passengerinfo Struct in DB
typedef struct
{
	const char *passengerid;
	int batterylevel;
	int rssi;
	const char *sentinelID;
	const char *reportTime;
	int illumination;
}passengerInfo;

int init_socket();
void *process_recieve(void *arg);
void *process_accept();
void SQL_INSERT(passengerInfo DBform);






