#include "server.h"

#define PART 100*BYTE

int create_client_directory(char* directory_name)
{
	char full_path[MAX_PATH_SIZE] = "";
	sprintf(full_path, "%s/%s", current_dir, directory_name);
	return mkdir(full_path, PERM_DIRECTORY);
};
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
//Форма
	fprintf(to_write, "<HR>\r\n");
	fprintf(to_write, "<H2>Загрузить файл в текущую директорию:</H2>\r\n<PRE>\n");
	fprintf(to_write, "<form enctype=\"multipart/form-data\" method=\"post\">");
	fprintf(to_write, "<p><input type=\"file\" name=\"f\">");
	fprintf(to_write, "<input type=\"submit\" value=\"Отправить\"></p>");
	fprintf(to_write, "</form>\r\n"); 
	fprintf(to_write, "</PRE>\r\n<HR>\r\n<ADDRESS>%s ACOS-2015</ADDRESS>\r\n</BODY></HTML>\r\n", SERVER_NAME);
	closedir(directory);
	return 0;
}
int recvall(int sfd, void* buf, int len, int flags)
{
    int total = 0;
    while(total < len)
    {
        int n = recv(sfd, buf + total, len - total, flags);
        if(n == -1)
            return -1;
        total += n;
    }
    return total;
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
int perform_get(FILE* to_write, char* path, char* answer_protocol)
{
	char* fullpath = (char*)malloc(MAX_URL_SIZE*sizeof(char));
	protocol = answer_protocol;
	strcat(fullpath, current_dir);
	strcat(fullpath,path);

	struct stat fileinfo;
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
int str_copy_until_char(char* from, char* where, char stop)
{
	int i = 0;
	while (from[i] != stop)
	{
		 where[i] = from[i];
		 i++;
	}
	where[i] = '\0';
	return i;
}
int recv_until_str(int sock, char** buffer_begin, char* substring, int occurrence, int* total_received, int* buf_memory_allocated)
{
/* Получает данные в buffer_begin + total_received,
 * пока начиная с buffer_begin не найдет occurence-ое вхождение подстроки substring,
 * при недостатке уже аллоцированной памяти переаллоцирует её.
 * Работает только с динамически выделенной памятью, иначе - ошибка.
*/
	char* look_from = *buffer_begin, *temp_substr;
	int temp = 0, received_bytes = 0, finish = 0, occurence_count = 0;
	if (total_received == NULL) total_received = &temp;
	if (substring == NULL) substring = "\0";
	
	while(!finish)
	{
		if (temp_substr = strstr(look_from, substring))
		{
			look_from = temp_substr + strlen(temp_substr);
			if (++occurence_count == occurrence) finish = 1;
			continue;
		}
		int nbytes = recv(sock, *buffer_begin + *total_received, 1, 0);
		if (nbytes == -1) return -1;
		*total_received += nbytes;
		((char*)*buffer_begin)[*total_received] = '\0';
		if (*buf_memory_allocated - *total_received - 1 < PART)
		{
			*buffer_begin = (char*)realloc(*buffer_begin, *buf_memory_allocated *= 2);
			look_from = *buffer_begin;
		}
		received_bytes += nbytes;
	}
	return received_bytes;
}
int recv_until_nbytes(int sock, char** buffer_begin, int bytes, int* total_received, int* buf_memory_allocated)
{
	int temp = 0, received_bytes = 0, memory = 1;
	if (total_received == NULL) total_received = &temp;
	
	while(received_bytes < bytes)
	{
		int nbytes = recv(sock, *buffer_begin + *total_received, 1, 0);
		if (nbytes == -1) return -1;
		*total_received += nbytes;
		((char*)*buffer_begin)[*total_received] = '\0';
		if (*buf_memory_allocated - *total_received - 1 < PART)
			*buffer_begin = (char*)realloc(*buffer_begin, *buf_memory_allocated *= 2);
		received_bytes += nbytes;
	}
	return received_bytes;
}


char* nstrstr(const char *haystack, const char *needle, int occurrence)
{
	if (occurrence <= 0) return NULL;
	
	int iterator, length = strlen(needle); 
	char *temp = (char*)haystack;
	
	for (iterator = 0; iterator < occurrence && temp != NULL; ++iterator)
		temp = strstr(temp, needle) + length;

	return temp == NULL ? NULL : temp - length;
}
	
int perform_post(int sock, char** buffer, int* total_received, int* buf_memory_allocated)
{

	int 	local_total_received = 0,
			content_length, nbytes,
			file_size;
	
	char	referrer[4096],	*referer_pointer,			*first_double_slash,	*file_begin,
			boundary[4096],	*boundary_pointer,			*second_double_slash,	*file_end,
			length[50],		*content_length_pointer,	*third_double_slash,	 
			filename[1024],	*filename_pointer,
			nth; 
	
	recv_until_str(sock, buffer, "\r\n\r\n", 1, total_received, buf_memory_allocated);
	first_double_slash = *buffer + *total_received; // == '\0'
	
	referer_pointer = strstr(*buffer, "Referrer: ");
	boundary_pointer = strstr(*buffer, "boundary=");
	content_length_pointer = strstr(*buffer, "Content-Length: ");
	
	if (referer_pointer)
		str_copy_until_char(referer_pointer + strlen("Referrer: "), referrer, '\r');
	if (boundary_pointer)
		str_copy_until_char(boundary_pointer + strlen("boundary="), boundary, '\r');
	if (content_length_pointer)
	{
		str_copy_until_char(content_length_pointer + strlen("Content_length: "), length, '\r');
		content_length = atoi(length);
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
		FILE* new_file = fopen(filepath, "wb");
		fwrite(file_begin, 1, content_length, new_file);
		fclose(new_file);
		post_ok(to_write, referrer);
	}
	else 
	{
		printf("File %s exists. REWRITE MODE %s", filename, REWRITE ? "ON" : "OFF");
		if (REWRITE)
		{
			FILE* new_file = fopen(filepath, "wb");
			fwrite(file_begin, 1, content_length, new_file);
			fclose(new_file); 
			post_ok(to_write, referrer);
		}
		else
			post_failed(to_write, referrer);
	}
	fclose(to_write);
}

int request_processing(int sock)
{
	int nbytes = 0, total = 0, memory = 100;
	char *buffer = (char*)malloc(100*sizeof(char)), method[5], path[4096], our_protocol[20];
	protocol = our_protocol;	//Связывание глобальной переменной
	
	buffer[0] = '\0';
	recv_until_str(sock, &buffer, "\r\n", 1, &total, &memory);
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
		perform_post(sock, &buffer, &total, &memory);
	else
	{
		FILE *to_write = fdopen(sock, "a+");
		throw_error(to_write, 501, title_501, "Unknown method.");
		fclose(to_write);
		return 501;
	}
	
	return 0;
}

void* thread_processing(void *our_pipe)//править
{	
	int* temp = (int*)our_pipe;
	int read_fd = temp[0];
	void* request = malloc(MAX_REQUEST_SIZE);
	
	while(true)
	{
		int client_socket;
		pthread_mutex_lock(&mtx);								
		while (sizeof(int) != read(read_fd, &client_socket, sizeof(int)));
		pthread_mutex_unlock(&mtx);	
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
		pthread_create(&thread_id, NULL, thread_processing, (void*)our_pipe);
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
        perror("setsock error");
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
	printf("Сервер запущен...\n");
	return server_socket;
}
int main(int argc, char** argv)
{
	current_dir = getenv("PWD");

	if (argc != 1) 
		exit(EXIT_FAILURE);
		
	setlocale(LC_ALL, "utf-8");
	
	int server_fd;	
	setbuf(stdout, NULL);
	server_fd = run_server(PORT);	
	printf("Server is working on port %d\n", PORT);
	run_handling_requests(server_fd);
		return 0;
}
