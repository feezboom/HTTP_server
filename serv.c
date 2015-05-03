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
#include <assert.h>

#define THREADS_NUMBER 10

#define PORT 80
#define MAX_REQUEST_SIZE 1024

#define OK 200
#define POST_OK 201
#define GET_HEAD_OK 202
#define BAD_REQUEST 400
#define NOT_FOUND 404

int run_server(char* server_dir, int server_port);
int run_handling_requests(int server_fd);
int parse(char* request, char* path);
int respond(char* request);
int perform_request(int client, char* request);
void* thread_processing(void *client_number);



/* Вид запроса: "GET path HTTP/version\r\n\r\n" 
 * Вместо version 1.0, or 1,1
 * */
 
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
 
 
int perform_request(int client, char* request)
{
	 /*
	  * 1) Отдача статики, т.е., как я понял, запрашивается файл, конкретный
	  * 2) Поддержка скриптов через cgi - запуск скрипта?????
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
	//Здесь должно быть описано ожидание потоком момента, пока в пайп
	//не будут записаны данные
		 
//		perform_request(client, request);

	return our_pipe;
}
 
 
int run_handling_requests(int server_fd)	//Функция обработки запросов
{
	int* our_pipe = run_threads(THREADS_NUMBER);
	while (0 == 0)
	{
		int client = accept(server_fd, NULL, 0);
		assert(client >=0);					//Как бы проверка на ошибку accept
											//На этом этапе к нам поступил очередной запрос...
		char buffer[MAX_REQUEST_SIZE], *request;
		ssize_t received_bytes = recv(client, buffer, MAX_REQUEST_SIZE, 0);
		assert(received_bytes >=0);			//Проверка с отловом ошибки, если нужно
		buffer[received_bytes] = '\0'; 		//Запрос без символа-терминатора строки...
		request = buffer;
		
											//Уже обработанный мы кладём в неименованый канал(пайп).
		write(our_pipe[1], request, received_bytes + 1);
//Далее все заботы уже на потоках, здесь пока вроде всё...
	}
	return 0;
 }


int run_server(char* server_dir, int server_port)
{
	int server;
	struct sockaddr_in sa;
	
//Preparing server for working with requests	
	server = socket(AF_INET, SOCK_STREAM, 0);		//Creating socket
	
	sa.sin_family = AF_INET;
	sa.sin_port = htons(server_port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
//INADDR_ANY - все адреса локального хоста (0.0.0.0)
//INADDR_LOOPBACK - адрес интерфейса (127.0.0.1)
//INADDR_BROADCAST - широковещательный адрес (255.255.255.255)
//htonl, htons - учитывают порядок следования байтов в системе, little/big endian, и делают всё как надо

	bind(server, (struct sockaddr *)&sa, sizeof(sa));		//Binding - assotiation server_socket_fd with sa?
	listen(server, 10);										//listening with maximum 10 requests in queue
//Preparing server finished, server is listening...
	return server;
}


int main(int argc, char** argv)
{
	if (argc != 3) 
		exit(EXIT_FAILURE);
	
	char *current_dir = argv[1];
	int server_fd;
//	int port = atoi(argv[2]);
	
	server_fd = run_server(current_dir, PORT);	
//Теперь сервер готов к приёму входящих соединений
	run_handling_requests(server_fd);
//Запускаем обработку входящих соединений сервера server_fd
	
	
	
	return 0;
}
