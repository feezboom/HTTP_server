#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/un.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <locale.h>
#include <assert.h>
#include <limits.h>

#define BYTE sizeof(char)
#define KBYTE  BYTE*1024
#define MBYTE KBYTE*1024
#define GBYTE MBYTE*1024
#define TBYTE GBYTE*1024

#define PERM_DIRECTORY  S_IRWXU
#define PERM_FILE      (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define SERVER_NAME "Bug Server 1.00001 2001"

#define REWRITE 1

typedef int
#define true 1
#define false 0
bool;

#define PORT 8080
#define THREADS_NUMBER 10
#define REQUESTS_IN_QUEUE 10
static const int PART = 100*BYTE;
#define PART_SIZE KBYTE*50
#define MAX_REQUEST_SIZE 10*MBYTE
#define MAX_URL_SIZE 4096
#define MAX_BUFFER_SIZE 4096
#define MAX_FILE_SIZE 1024*1024*3
#define MAX_PATH_SIZE 1024

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

const char title_400[] = "Bad Request";
const char title_500[] = "Internal Server Error";
const char title_501[] = "Not Implemented";
const char title_403[] = "Forbidden";
const char title_404[] = "Not Found";
const char title_200[] = "OK";

const char post_respond_ok[]  		= "<HTML><HEAD><meta charset=\"utf-8\"><TITLE>OK</TITLE></HEAD>\r\n<BODY><H4>Файл успешно загружен.<a href=\"%s\">Вернуться назад.\r\n<H4></BODY></HTML>\r\n";
const char post_respond_failed[]  	= "<HTML><HEAD><meta charset=\"utf-8\"><TITLE>OK</TITLE></HEAD>\r\n<BODY><H4>Ошибка загрузки.<a href=\"%s\">Вернуться назад.\r\n<H4></BODY></HTML>\r\n";

char* protocol;
char* current_dir;





