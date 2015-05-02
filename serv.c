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


#define PORT 80
#define MAX_REQUEST_SIZE 1024

#define OK 200
#define POST_OK 201
#define GET_HEAD_OK 202
#define BAD_REQUEST 400
#define NOT_FOUND 404

void run_server(char* server_dir, int server_port);
int run_handling_requests(int server_fd);
int parse(char* request, char* path);
int respond(char* request);
int perform_request(int client, char* request);
void* client_processing(void *client_number);



/* Вид запроса: "GET path HTTP/version\r\n\r\n" 
 * Вместо version 1.0, or 1,1
 * */
 
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
 
 
 
 void* client_processing(void *client_number)
{
/*Этот кусок кода должен быть в отдельном потоке реализован*/		
/*		char buffer[MAX_REQUEST_SIZE], *request;
		ssize_t received_bytes = recv(client, buffer, MAX_REQUEST_SIZE, 0);
		assert(received_bytes >=0);//Проверка с отловом ошибки, если нужно
		buffer[received_bytes] = '\0';
		request = buffer;	 
		perform_request(client, request);
*/
/*Этот кусок кода должен быть в отдельном потоке реализован*/
	return client_number;
}
 int run_handling_requests(int server_fd)//Функция обработки запросов
 {
	 int clients[2]; clients[0] = 0;
//clients[0] - Количество клиентов, обрабатывающихся в данный момент
//clients[1] - Идентификатор текущего клиента (дескриптор сокета)
	 while (0 == 0)
	 {
		 //если, вдруг, клиентов слишком много, то добавить в очередь
		 //в очереди ждем, пока clients[0] не станет подходящей, чтобы начать обработку нового клиента
		int client = accept(server_fd, NULL, 0);
		assert(client >=0);					//Как бы проверка на ошибку accept
		clients[0]++;						//Присоединился один клиент
		//И здесь мы говорим: "А давайте засунем обработку этого клиента в отдельный поток"
		
		pthread_t thread_id;				//Если создание потока успешно:
		pthread_mutex_t mtx;
		pthread_cond_t decreased;
		pthread_create(&thread_id, NULL, client_processing, (void*)clients);	
		/*pthread_create(Идентификатор потока, атрибуты, функция запуска, аргумент функции запуска)*/	
	 }
	 return 0;
 }






void run_server(char* server_dir, int server_port)
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
}




int main(int argc, char** argv)
{
	if (argc != 3) 
		exit(EXIT_FAILURE);
	
	char *current_dir = argv[1];
//	int port = atoi(argv[2]);
	
	run_server(current_dir, PORT);
	//Here our sever is ready to receive requests...
	
	
	
	return 0;
}
