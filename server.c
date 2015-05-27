#include "server.h"
#include "functions.h"

int thread__ID = 0;

int perform_post(int sock, char** buffer, int* total_received, int* buf_memory_allocated)
{

	int 	local_total_received = 0,
			content_length, nbytes,
			file_size;
	
	char	referrer[4096],	*referer_pointer,			*first_double_slash,	*file_begin,
			boundary[4096],	*boundary_pointer,			*second_double_slash,	*file_end,
			length[50],		*content_length_pointer,	*third_double_slash,	write_mode[3],
			filename[1024],	*filename_pointer,			*accept_pointer,		accept_type[30],
			nth; 
	
	recv_until_str(sock, buffer, "\r\n\r\n", 1, total_received, buf_memory_allocated);
	first_double_slash = *buffer + *total_received;
	
	referer_pointer = strstr(*buffer, "Referer: ");
	boundary_pointer = strstr(*buffer, "boundary=");
	content_length_pointer = strstr(*buffer, "Content-Length: ");
	accept_pointer = strstr(*buffer, "Accept: ");
	
	if (referer_pointer)
		str_copy_until_char(referer_pointer + strlen("Referer: "), referrer, '\r');
	if (boundary_pointer)
		str_copy_until_char(boundary_pointer + strlen("boundary="), boundary, '\r');
	if (content_length_pointer)
	{
		str_copy_until_char(content_length_pointer + strlen("Content_length: "), length, '\r');
		content_length = atoi(length);
	}
	if (accept_pointer)
	{
		str_copy_until_char(accept_pointer + strlen("Accept: "), accept_type, ',');
		strcasecmp("text/html", accept_type) ? strcpy(write_mode, "wb") : strcpy(write_mode, "w");
	}
	
	recv_until_nbytes(sock, buffer, content_length, total_received, buf_memory_allocated);
	
	filename_pointer = strstr(*buffer, "filename=");
	if (filename_pointer)
		str_copy_until_char(filename_pointer + strlen("filename=\""), filename, '"');
	
	file_begin = strstr(filename_pointer, "\r\n\r\n") + strlen("\r\n\r\n");
	file_end = *buffer + *total_received - strlen(boundary) - strlen("\r\n\r\n") * 2 - strlen("\n");
	file_size = file_end - file_begin + 1;
	
	
//На данном этапе весь файл получен!!!:)
	char  filepath[MAX_PATH_SIZE];									//Составляем полный путь к создаваемому файлу
	sprintf(filepath, "%s/clients_files/%s", current_dir, filename);
	
	struct stat fileinfo;
	FILE* to_write = fdopen(sock, "a+");
	if (stat(filepath, &fileinfo) < 0)
	{
		printf("File %s doesn't exist\n", filename);
		FILE* new_file = fopen(filepath, write_mode);
		fwrite(file_begin, 1, file_size, new_file);
		fclose(new_file);
		post_ok(to_write, referrer);
	}
	else 
	{
		printf("File %s exists. REWRITE MODE %s. ", filename, REWRITE ? "ON" : "OFF");
		if (REWRITE)
		{
			FILE* new_file = fopen(filepath, write_mode);
			fwrite(file_begin, 1, file_size, new_file);
			fclose(new_file); 
			post_ok(to_write, referrer);
		}
		else
			post_failed(to_write, referrer);
	}
	fclose(to_write);
}
int perform_get(FILE* to_write, char* path, char* answer_protocol)
{
	char* fullpath = (char*)malloc(MAX_URL_SIZE);
	protocol = answer_protocol;
	
	sprintf(fullpath, "%s%s", current_dir, path);
	
	struct stat fileinfo;
	if (stat(fullpath, &fileinfo) < 0)
	{
		char msg[1024];
		sprintf(msg, "Requested resourse %s not found.", fullpath);
		throw_error(to_write, 404, title_404, msg);
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
		
		if (stat(buf, &fileinfo) >= 0)
		{
			int result = throw_file(to_write, buf, fileinfo);
			free(fullpath);
			return result;
		}
		else
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

int request_processing(int sock)
{
	int nbytes = 0, total = 0, memory = PART + 1;
	char *buffer = (char*)malloc(memory), method[5], path[4096], our_protocol[20];
	protocol = our_protocol;	//Связывание глобальной переменной
	
	buffer[0] = '\0';
	assert(memory >= total - PART);
	recv_until_str(sock, &buffer, "\r\n", 1, &total, &memory);
	assert(memory >= total - PART);
	print_current_time(); printf("%s", buffer);
	
	str_copy_until_char(buffer, method, ' ');
	str_copy_until_char(strstr(buffer, "/"), path, ' ');
	str_copy_until_char(strstr(buffer, "HTTP"), protocol, '\r');
	
//На этот момент получена первая строка запроса (метод, директория, протокол)	
	
	if (!strcasecmp("GET", method))
	{
		recv_until_str(sock, &buffer, "\r\n\r\n", 1, &total, &memory);
		FILE *to_write = fdopen(sock, "a+");
		perform_get(to_write, path, protocol);
		fclose(to_write);
	}
	else if (!strcasecmp("POST", method))
	{
		perform_post(sock, &buffer, &total, &memory);
		assert(memory >= total - PART);
	}
	else
	{
		FILE *to_write = fdopen(sock, "a+");
		throw_error(to_write, 501, title_501, "Unknown method.");
		fclose(to_write);
		free(buffer);
		return 501;
	}
	
	free(buffer);
	return 0;
}
void* thread_processing(void *our_pipe)
{	
	int* temp = (int*)our_pipe, my_id = thread__ID++;
	int read_fd = temp[0];
//	printf("THREAD_ID = %d started\n", my_id);
	while(true)
	{
		int client_socket;
		pthread_mutex_lock(&mtx);								
		while (sizeof(int) != read(read_fd, &client_socket, sizeof(int)));
		pthread_mutex_unlock(&mtx);	
//		printf("Request by ID(%d) ", my_id);
		request_processing(client_socket);
	}
	return our_pipe;
}

int* run_threads(int threads_number)
{
	assert(threads_number > 0);
	 
	int i; int* our_pipe = (int*)malloc(2*sizeof(int));
	pipe(our_pipe);
	
	pthread_t thread_id;	
	for (i = 0; i < threads_number; ++i)
	{
		pthread_create(&thread_id, NULL, thread_processing, (void*)our_pipe);
	}
	return our_pipe;
 }
int run_handling_requests(int server_fd)
{
	int* our_pipe = run_threads(THREADS_NUMBER);
	while (0 == 0)
	{
		int client = accept(server_fd, NULL, 0);
		assert(client >=0);
		write(our_pipe[1], &client, sizeof(int));
	}
	return 0;
 }
int run_server(int server_port)
{
	int server_socket;
	struct sockaddr_in sa;
	
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	sa.sin_family = AF_INET;
	sa.sin_port = htons(server_port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	int optval = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    {
        perror("Setsockopt error");
        exit(EXIT_FAILURE);
    }
	if (bind(server_socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
	{
		perror("BIND ERROR");
		exit(EXIT_FAILURE);
	}
	if (listen(server_socket, REQUESTS_IN_QUEUE) == -1)
	{
		perror("LISTEN ERROR");
		exit(EXIT_FAILURE);
	}
	create_client_directory("clients_files");
	printf("Server started...\n");
	return server_socket;
}


int main(int argc, char** argv)
{
	setlocale(LC_ALL, "utf-8");
	current_dir = getenv("PWD");

	if (argc != 1)
	{
		printf("Server doesn't need any parameters! I'm shuting down...");
		exit(EXIT_FAILURE);
	}
	
	int server_fd;	
	setbuf(stdout, NULL);
	server_fd = run_server(PORT);	
	printf("Server is working on port %d\n", PORT);
	run_handling_requests(server_fd);
	return 0;
}
