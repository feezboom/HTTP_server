/*
 * Логика
 * -Сначала мы запускаем наш сервер для готовности приема входящих соединений(listen)
 * 
 * -После этого мы сразу же запускаем столько потоков, 
 * сколько нам нужно максимум одновременно обрабатывающихся клиентов.(run_threads)
 * Здесь же мы создаем неименованый канал, чтобы класть туда текущие,
 * неудовлетворённые запросы.
 * 
 * -Далее мы ждем входящее соединение (accept)
 * 
 * -Как только дождались кого-нибудь, кладем наш запрос в созданный нами ранее пайп.
 * 
 * -Потоки же в это время следят за этим пайпом, готовые прочитать запрос из него,
 * как только он там появится, и удовлетворить его.
 * */
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

#define BYTE 1
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

#define THREADS_NUMBER 1
#define REQUESTS_IN_QUEUE 10

#define PORT 8080

#define PART_SIZE KBYTE*50
#define MAX_REQUEST_SIZE 10*MBYTE
#define MAX_URL_SIZE 4096
#define MAX_BUFFER_SIZE 4096
#define MAX_FILE_SIZE 1024*1024*3
#define MAX_PATH_SIZE 1024

char* current_dir; //Это текущая директория

int create_client_directory(char* directory_name)
{
	char full_path[MAX_PATH_SIZE] = "";
	sprintf(full_path, "%s/%s", current_dir, directory_name);
	return mkdir(full_path, PERM_DIRECTORY);
};

/*Titles*/
const char title_400[] = "Bad Request";
const char title_500[] = "Internal Server Error";
const char title_501[] = "Not Implemented";
const char title_403[] = "Forbidden";
const char title_404[] = "Not Found";
const char title_200[] = "OK";
/*Titles*/

const char post_respond_ok[]  		= "<HTML><HEAD><meta charset=\"utf-8\"><TITLE>OK</TITLE></HEAD>\r\n<BODY><H4>Файл успешно загружен.<a href=\"%s\">Вернуться назад.\r\n<H4></BODY></HTML>\r\n";
const char post_respond_failed[]  	= "<HTML><HEAD><meta charset=\"utf-8\"><TITLE>OK</TITLE></HEAD>\r\n<BODY><H4>Ошибка загрузки.<a href=\"%s\">Вернуться назад.\r\n<H4></BODY></HTML>\r\n";


//Структура сервера
int main(int argc, char** argv);
	int run_server(int server_port);						//Запуск сервера: до состояния listen
	int run_handling_requests(int server_fd);				//Работает с входящими соединениями, делает всё
		int* run_threads(int threads_number);					//Запуск потоков
			void* thread_processing(void *client_number);			//Функция рабоsты каждого потока
				int get_request(int client_socket, void* buffer);			//Принять запрос от клиента
				int perform_request(FILE* to_send, void* request, int request_len);			//Исполнить запрос клиента

int perform_request(FILE* to_send, void* request, int request_len);			//Исполнить запрос клиента
	int parse(FILE* to_write,const void* our_request, int request_len);
		int perform_get(FILE* to_write, char* path, char* protocol);
			int show_directory(FILE* to_write, const char* fullpath);
			int throw_file(FILE* to_write, const char* path, struct stat fileinfo);
			int throw_error(FILE* to_write, int code, const char* title, const char* msg);
				int throw_headers(FILE* to_write, int code, const char* title, const char* content_type, const char* protocol);
				char* get_content_type(const char* name);
				void print_current_time();

/* Вид запроса: "Request-Line [ General-Header | Request-Header | Entity-Header ]\r\n[ Entity-Body ]" 
 * 
 * Краткое руководство по запросам...
 * http://phpclub.ru/detail/article/http_request 
 * 
 * Типичные запросы
 * GET http://www.site.ru/news.html HTTP/1.0\r\n 
 *	Host: www.site.ru\r\n
 *	Referer: http://www.site.ru/index.html\r\n
 *	Cookie: income=1\r\n
 *	\r\n
 * 
 * POST http://www.site.ru/news.html HTTP/1.0\r\n 
 *	Host: www.site.ru\r\n 
 *	Referer: http://www.site.ru/index.html\r\n 
 *	Cookie: income=1\r\n 
 *	Content-Type: application/x-www-form-urlencoded\r\n
 *	Content-Length: 35\r\n
 *	\r\n
 *	login=Petya%20Vasechkin&password=qq
 * 
 * Ответ, например, должен отправляться в качестве html странички.
*/
 
char* protocol;
void print_current_time()
{
	char* str_time;
	time_t current_time;
	time(&current_time);
	str_time=ctime(&current_time);
	str_time[strlen(str_time)-1]='\0';
	printf("%s:",str_time);
}
char* get_content_type(const char* name)
{
  char *ext = strrchr(name, '.');
  if (!ext) return NULL;
  if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) return "html";
  if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
  if (strcmp(ext, ".gif") == 0) return "image/gif";
  if (strcmp(ext, ".png") == 0) return "image/png";
  if (strcmp(ext, ".css") == 0) return "text/css";
  if (strcmp(ext, ".au") == 0) return "audio/basic";
  if (strcmp(ext, ".wav") == 0) return "audio/wav";
  if (strcmp(ext, ".avi") == 0) return "video/x-msvideo";
  if (strcmp(ext, ".mpeg") == 0 || strcmp(ext, ".mpg") == 0) return "video/mpeg";
  if (strcmp(ext, ".mp3") == 0) return "audio/mpeg";
  return NULL;
}
int throw_error(FILE* to_write, int code, const char* title, const char* msg)
{
	if(code != 200) 
	{	
		throw_headers(to_write, code, title, "html/text", protocol);
		fprintf(to_write,"<HTML><HEAD><TITLE>%d %s</TITLE></HEAD>\r\n<BODY><H4>Error %d %s: %s</H4></BODY></HTML>\r\n", code, title, code, title, msg);
		print_current_time();
		printf("Ошибка %d: %s\n",code, msg);
	} 
	return(0);
}
int throw_headers(FILE* to_write, int code, const char* title, const char* content_type, const char* protocol)
{
	fprintf(to_write, "%s %d %s\r\n", protocol, code, title);
	fprintf(to_write, "Server: %s\r\n", SERVER_NAME);
	if (content_type) fprintf(to_write, "Content-Type: %s\r\n", content_type);
	fprintf(to_write, "Connection: close\r\n");
	fprintf(to_write, "\r\n");
	return 0;
}
int throw_file(FILE* to_write, const char* path, struct stat fileinfo)
{
	char data[MAX_BUFFER_SIZE];
	int i = 0;
	
	FILE *our_file = fopen(path, "r");
	if (our_file == NULL)
	{
		char error_text[1024];
		sprintf(error_text, "Forbidden. Access to file \"%s\" denied.", path);
		throw_error(to_write, 403, title_403, error_text);
		return 403;
	}
	throw_headers(to_write, 200, "OK", get_content_type(path), protocol);
	while ((i = fread(data, 1, sizeof(data), our_file)) > 0) 
		fwrite(data, 1, i, to_write);
		
	return 200;
}
int show_directory(FILE* to_write, const char* fullpath)
{
	DIR* directory;
	struct dirent* item;
	struct stat item_info;
	char buf[MAX_BUFFER_SIZE];
	
	directory = opendir(fullpath);
	throw_headers(to_write, 200, "OK", "html", protocol);
	
	fprintf(to_write, "<HTML><HEAD><meta charset=\"utf-8\"><TITLE>DIRECTORY: %s</TITLE></HEAD>\r\n<BODY>", fullpath);     
	fprintf(to_write, "<H4>DIRETORY: %s</H4>\r\n<PRE>\n", fullpath);
	fprintf(to_write, "Item\r\n");
	fprintf(to_write, "<HR>\r\n");
//   <A HREF="ITEM_NAME">LINK_TO_ITEM</A>
	fprintf(to_write, "<A HREF=\"..\">..</A>\r\n");
	while((item = readdir(directory)))
	{
		if(!strcmp(item->d_name, ".") || !strcmp(item->d_name, "..") || (item->d_name[0] == '.'))
			continue;
		strcpy(buf, fullpath);
		if (buf[strlen(fullpath) - 1] != '/')
			strcat(buf, "/");
		strcat(buf, item->d_name);
		
		if(stat(buf, &item_info) >= 0)
		{
			char* slash = S_ISDIR(item_info.st_mode) ? "/" : "";
			fprintf(to_write, "<A HREF=\"%s%s\">%s%s</A>\r\n", item->d_name, slash, buf, slash);
		}
	}
//А здесь вставим форму для отправки файлов методом POST
	fprintf(to_write, "<HR>\r\n");
	fprintf(to_write, "<H2>Загрузить файл в текущую директорию:</H2>\r\n<PRE>\n");
	fprintf(to_write, "<form enctype=\"multipart/form-data\" method=\"post\">");
	fprintf(to_write, "<p><input type=\"file\" name=\"f\">");
	fprintf(to_write, "<input type=\"submit\" value=\"Отправить\"></p>");
	fprintf(to_write, "</form>\r\n"); 
//Конец формы
	fprintf(to_write, "</PRE>\r\n<HR>\r\n<ADDRESS>%s ACOS-2015</ADDRESS>\r\n</BODY></HTML>\r\n", SERVER_NAME);
	closedir(directory);
	return 0;
}
void post_ok(FILE* to_write, char* refferer)
{
	throw_headers(to_write, 200, title_200, "html", protocol);
	fprintf(to_write, post_respond_ok, refferer);
}
void post_failed(FILE* to_write, char* refferer)
{
	throw_headers(to_write, 500, title_500, "html", protocol);
	fprintf(to_write, post_respond_failed, refferer);
}

int perform_post(FILE* to_write, const void* request)
{
	void* memory = malloc(MAX_REQUEST_SIZE);
//	memcpy(memory, request, len);
	
	char *length_pointer 	= strstr(request, "Content-Length: ") 	+ strlen("Content-Length: ");
	char *boundary_pointer 	= strstr(request, "boundary=") 			+ strlen("boundary=");
	char *filename_pointer	= strstr(request, "filename=")			+ strlen("filename=\"");
	char *refferer_pointer 	= strstr(request, "Referer: ")			+ strlen("Referer: ");
	char *data_begin		= strstr(filename_pointer, "\r\n\r\n")	+ strlen("\r\n\r\n");
//	char *data_end			= strstr(data_begin, "\r\n");
	
	int content_length;
	sscanf(length_pointer, "%d", &content_length);					//Выделяем нужную информацию из хэдэров
	char *boundary = strtok(boundary_pointer, "\r");
	char *filename = strtok(filename_pointer, "\"\r\n");
	char *refferer = strtok(refferer_pointer, "\r\n");	
	
	void *data_end			= (void*)strstr(data_begin, boundary) - 200;
	data_end = '\0';
	
	
	printf("CONTENTLEN = %d\n", content_length);
//	printf("content=***%s***", data_begin);
	printf("ALLRIGHT***************\n");
	
	char  filepath[MAX_PATH_SIZE];									//Составляем полный путь к создаваемому файлу
	sprintf(filepath, "%s/clients_files/%s", current_dir, filename);
	
	struct stat fileinfo;
	if (stat(filepath, &fileinfo) < 0)
	{
		printf("File doesn't exist\n");
		FILE* new_file = fopen(filepath, "wb");
		fwrite(data_begin, 1, content_length, new_file);
		fclose(new_file);
		post_ok(to_write, refferer);
	}
	else 
	{
		printf("File exists. REWRITE MODE %s\n", REWRITE ? "ON" : "OFF");
		if (REWRITE)
		{
			FILE* new_file = fopen(filepath, "wb");
			fwrite(data_begin, 1, content_length, new_file);
			fclose(new_file); 
			post_ok(to_write, refferer);
		}
		else
			post_failed(to_write, refferer);
	}
}
int perform_get(FILE* to_write, char* path, char* protocol)
{
	char* fullpath = (char*)malloc(MAX_URL_SIZE*sizeof(char));//Полный путь к запр. ресурсу
		 
//Склеиваем полный путь к запрашиваемому ресурсу
	strcat(fullpath, current_dir);
	strcat(fullpath,path);

	struct stat fileinfo;
//Проверка на существование
	if (stat(fullpath, &fileinfo) < 0)
	{
		throw_error(to_write, 404, title_404, "Requested resourse not found.");
		free(fullpath);
		return 404;
	}
	
	if (S_ISDIR(fileinfo.st_mode))
	{
		char buf[MAX_BUFFER_SIZE];
		sprintf(buf, "%s", fullpath);
		
		int len = strlen(fullpath);
		if (len == 0 || fullpath[len-1] != '/') 
			strcat(buf, "/");
		strcat(buf, "index.html");
		
		if (stat(buf, &fileinfo) >= 0)//Значит файл index.html существует, отправим его
		{
			int result = throw_file(to_write, buf, fileinfo);
			free(fullpath);
			return result;
		}
		else 	//значит файл index.html не существует, выведем список содержимого директории
		{
			int result = show_directory(to_write, fullpath);
			free(fullpath);
			return result;
		}
	}
	if (S_ISREG(fileinfo.st_mode))
	{
		int result = throw_file(to_write, fullpath, fileinfo);
		free(fullpath);
		return result;
	}
	return -1;
}
int parse(FILE* to_write,const void* our_request, int request_len)
{
	
	
	void* request = malloc(request_len + 1);
	memcpy(request, our_request, request_len);
	((char*)request)[request_len] = '\0';
	printf("\n\n\n\n\n**********BEGIN%sEND***********\n\n\n\n\n", (char*)request);
	
//	char *for_post	= strstr(request, "\r\n")	+ strlen("\r\n");
/*Выделение метода, uri, протокола*/
	char *method;
	char *path;
	if (!request || strlen(request) < 3) return -1;
	
	char* head = strtok(request, "\r");
	print_current_time();
	printf("REQUEST:%s\n", head);
	method = strtok(request, " ");
	path = strtok(NULL, " ");
	protocol = strtok(NULL, "\r");
	if (!method || !path || !protocol) return -1;
/*Выделение метода, uri, протокола*/

	if(!strcasecmp("GET", method))
		perform_get(to_write, path, protocol);
	else 
	if(!strcasecmp("POST", method))
	{
//		memcpy(request, our_request, request_len);
//		((char*)request)[request_len] = '\0';
//		printf("REQ****BEGIN%sEND*****\n", (char*)request);
		perform_post(to_write, our_request);
	}
	else
	{
		throw_error(to_write, 501, title_501, "Unknown method.");
		return 501;
	}
	return 0;
}
int recvall(int sfd, void* buf, int len, int flags)
{
    int total = 0;
    while(total < len)
    {
        int n = recv(sfd, buf + total, len - total, flags);
        if(n ==  0) return total; 
        if(n == -1) return -1;
        total += n;
    }
    return total;
}

int get_request(int client_socket, void* buffer)//recieve all, не знаем размера
{
	long long buffer_size = PART_SIZE, nbytes, received = 0;
	nbytes = recv(client_socket, (void*)buffer, PART_SIZE, 0);
	received = nbytes;
	while (100500)
	{
		nbytes = recvall(client_socket, (void*)buffer + received, PART_SIZE, MSG_DONTWAIT);
		if (nbytes  >  0) 
		{
//			buffer_size *= 2;
			received += nbytes;
		}
		if (nbytes <=  0)
		{
//			buffer[received] = '\0';
			return received;
		}
	}
	return received;
}
int perform_request(FILE* to_send, void* request, int request_len)
{
	parse(to_send, request, request_len);
	 /*
	  * 1) Отдача статики, т.е., как я понял, запрашивается файл, конкретный
	  * 2) Поддержка скриптов через cgi
	  * 3) Возможность загрузки файлов в POST запросах
	  * 
	  * Типа именно здесь мы должны ответить нашему клиенту, отправить ему то, что он запросил
	 */
	 return 0;
 }
 /*MUTEX!*/
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
void* thread_processing(void *our_pipe)//править
{	
//Здесь описано ожидание потоком момента, пока в пайп
//не будут записаны данные, и соответственно чтение этих данных

	int* temp = (int*)our_pipe;
	int read_fd = temp[0];
	void* request = malloc(MAX_REQUEST_SIZE);
	
	while(true)
	{
		int client_socket;
//Блокируем доступ другим потокам...
		pthread_mutex_lock(&mtx);								

//Читаем до тех пор, пока не прочитаем что нибудь...
		while (sizeof(int) != read(read_fd, &client_socket, sizeof(int)));

//Даем возможность другим потокам что-нибудь прочитать...
		pthread_mutex_unlock(&mtx);	

//Далее, просто, ПОЛУЧАЕМ, обрабатываем, выполняем запрос(его распарсивание и исполнение).
		int request_len = get_request(client_socket, request);
		if (request_len <= 0)
			continue;			//Какая-то ошибка принятия запроса
//		((char*)request)[request_len] = '\0';
//		printf("*******%s***END", request);
//		exit(1);
//После - обработка запроса(парсинг), и ответ на него
		FILE* to_send = fdopen(client_socket, "a+");
		perform_request(to_send, request, request_len);
		fclose(to_send);
	}
	return our_pipe;
}
int* run_threads(int threads_number) //Пусть возвращает пайп для обмена с потоками данными...
{
	assert(threads_number > 0);
	 
	int i; int* our_pipe = (int*)malloc(2*sizeof(int));
	pipe(our_pipe);						//Создаём наш канал для очереди запросов
	
	pthread_t thread_id;				//Если создание потока успешно:		
	for (i = 0; i < threads_number; ++i)
		pthread_create(&thread_id, NULL, thread_processing, (void*)our_pipe);
		
	/*pthread_create(Идентификатор потока, атрибуты, функция запуска, аргумент функции запуска)*/
	
	return our_pipe;
 }
int run_handling_requests(int server_fd)	//Функция обработки запросов
{
//Запускаем кучу потоков
	int* our_pipe = run_threads(THREADS_NUMBER);
	while (0 == 0)
	{
		int client = accept(server_fd, NULL, 0);
		assert(client >=0);					//Как бы проверка на ошибку accept

//На этом этапе к нам поступил очередной запрос...
//Перенаправляем поступившего клиента в очередь к потокам.
		write(our_pipe[1], &client, sizeof(int));
//Далее все заботы уже на потоках, здесь пока вроде всё...
	}
	return 0;
 }
int run_server(int server_port)
{
	int server_socket;
	struct sockaddr_in sa;
	
//Preparing server for working with requests	
	server_socket = socket(AF_INET, SOCK_STREAM, 0);		//Creating socket
	
	sa.sin_family = AF_INET;
	sa.sin_port = htons(server_port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
//INADDR_ANY - все адреса локального хоста (0.0.0.0)
//INADDR_LOOPBACK - адрес интерфейса (127.0.0.1)
//INADDR_BROADCAST - широковещательный адрес (255.255.255.255)
//htonl, htons - учитывают порядок следования байтов в системе, little/big endian, и делают всё как надо
	int optval = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    {
        perror("setsock error");
        exit(EXIT_FAILURE);
    }

	if (bind(server_socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
	{
		perror("BIND ERROR");
		exit(EXIT_FAILURE);
	}
	if (listen(server_socket, REQUESTS_IN_QUEUE) == -1)						//listening with maximum REQUESTS_IN_QUEUE requests in queue
	{
		perror("LISTEN ERROR");
		exit(EXIT_FAILURE);
	}
	create_client_directory("clients_files");
	printf("Сервер запущен...\n");
//Preparing server finished, server is listening...
	return server_socket;
}


int main(int argc, char** argv)
{
	current_dir = getenv("PWD"); //Задание текущей директории(переменная глобальная)
	
	if (argc != 1) 
		exit(EXIT_FAILURE);
		
	setlocale(LC_ALL, "utf-8");
	
	int server_fd;
//	int port = atoi(argv[2]);		//зачем пользователем задается порт???
	
	setbuf(stdout, NULL);
	
	server_fd = run_server(PORT);	
//Теперь сервер готов к приёму входящих соединений
	printf("Server is working on port %d\n", PORT);
	run_handling_requests(server_fd);
//Запустили обработку входящих соединений к серверу server_fd, всё там...
	
		return 0;
}
