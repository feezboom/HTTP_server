//Не готова функция perform_request() совсем.

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
#include <assert.h>

#define SERVER_NAME "Bug Server 1.00001 2001"

typedef int
#define true 1
#define false 0
bool;

#define THREADS_NUMBER 10
#define REQUESTS_IN_QUEUE 10

#define PORT 8080

#define MAX_REQUEST_SIZE 1024
#define MAX_URL_SIZE 4096
#define MAX_BUFFER_SIZE 4096
#define MAX_FILE_SIZE 1024*1024*3

#define OK 200
#define POST_OK 201
#define GET_HEAD_OK 202
#define BAD_REQUEST 400
#define NOT_FOUND 404

//Структура сервера
int main(int argc, char** argv);
	int run_server(int server_port);						//Запуск сервера: до состояния listen
	int run_handling_requests(int server_fd);				//Работает с входящими соединениями, делает всё
		int* run_threads(int threads_number);					//Запуск потоков
			void* thread_processing(void *client_number);			//Функция рабоsты каждого потока
				char* get_request(int client_socket);					//Принять запрос от клиента
				int perform_request(FILE* to_send, char* request);			//Исполнить запрос клиента


/* Вид запроса: "Request-Line [ General-Header | Request-Header | Entity-Header ]\r\n[ Entity-Body ]" 
 * http://phpclub.ru/detail/article/http_request
 * */
char* get_request(int client_socket)
{
	int nbytes;
	char* req = (char*)malloc(MAX_REQUEST_SIZE*sizeof(char));
	if ( /**/(nbytes = recv(client_socket, (void*)req, MAX_REQUEST_SIZE - 1, 0))/**/ <= 0)
		return NULL;
	else 
	{
		req[nbytes] = '\0';
//		printf("our request:%s\n", req);
		return req;
	}
	
}
int perform_request(FILE* to_send, char* request)
{
//	parse(to_send, request);
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
void* thread_processing(void *our_pipe)
{	
//Здесь описано ожидание потоком момента, пока в пайп
//не будут записаны данные, и соответственно чтение этих данных

	int* temp = (int*)our_pipe;
	int read_fd = temp[0];
	char* request;
	
	while(true)
	{
		int client_socket;
//Блокируем доступ другим потокам...
		pthread_mutex_lock(&mtx);								

//Читаем до тех пор, пока не прочитаем что нибудь...
		while (sizeof(int) != read(read_fd, &client_socket, sizeof(int)));

//Даем возможность другим потокам что-нибудь прочитать...
		pthread_mutex_unlock(&mtx);	

//Далее, просто, получаем, обрабатываем, выполняем запрос(его распарсивание и исполнение).
		request = get_request(client_socket);
		if (request == NULL)
			continue;			//Какая-то ошибка чтения из пайпа
//После - обработка запроса(парсинг), и ответ на него
		FILE* to_send = fdopen(client_socket, "a+");
		perform_request(to_send, request);
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

	bind(server_socket, (struct sockaddr *)&sa, sizeof(sa));		//Binding - assotiation server_socket_fd with sa?
	listen(server_socket, REQUESTS_IN_QUEUE);						//listening with maximum REQUESTS_IN_QUEUE requests in queue
//Preparing server finished, server is listening...
	return server_socket;
}


int main(int argc, char** argv)
{
	if (argc != 2) 
		exit(EXIT_FAILURE);
	
	int server_fd;
//	int port = atoi(argv[2]);		//зачем пользователем задается порт???
	
	setbuf(stdout, NULL);
	
	server_fd = run_server(PORT);	
//Теперь сервер готов к приёму входящих соединений
	run_handling_requests(server_fd);
//Запустили обработку входящих соединений к серверу server_fd, всё там...
	
		return 0;
}
