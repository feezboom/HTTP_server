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

#define SERVER_NAME "Bugged Server 1.00001 2001"

#define MAX_URL_SIZE 4096
#define MAX_BUFFER_SIZE 4096
#define MAX_FILE_SIZE 1024*1024*3

char* protocol;
void print_current_time()
{
	char* str_time;
	time_t current_time;
	time(&current_time);
	str_time=ctime(&current_time);
	str_time[strlen(str_time)-1]='\0';
	printf("%s:\n",str_time);
}
char* get_content_type(const char* name)
{
  char *ext = strrchr(name, '.');
  if (!ext) return NULL;
  if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) return "text/html";
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
int throw_error(FILE* to_write, int err_code, const char* err_msg)
{
	if(err_code!=200) 
	{
		fprintf(to_write,"<HTML><HEAD><TITLE>Ошибка %d</TITLE></HEAD>\r\n <BODY><H4>Ошибка %d : %s</H4>\r\n </BODY></HTML>\r\n",err_code,err_code,err_msg);
		print_current_time();
		printf("Ошибка %d: %s\n",err_code, err_msg);
	} 
	return(0);
}
int throw_headers(FILE* to_write, int request_status, char* result, int length, char* content_type, char* protocol)
{
	fprintf(to_write, "%s %d %s\r\n", protocol, request_status, result);
	fprintf(to_write, "Server: %s\r\n", SERVER_NAME);
	if (content_type) fprintf(to_write, "Content-Type: %s\r\n", content_type);
	if (length >= 0) fprintf(to_write, "Content-Length: %d\r\n", length);
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
		throw_error(to_write, 403, error_text);
		return 403;
	}
	throw_headers(to_write, 200, "OK", fileinfo.st_size, get_content_type(path), protocol);
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
	throw_headers(to_write, 200, "OK", 2, "html/text", protocol);
	
	fprintf(to_write, "<HTML><HEAD><TITLE>DIRECTORY: %s</TITLE></HEAD>\r\n<BODY>", fullpath);     
	fprintf(to_write, "<H4>DIRETORY: %s</H4>\r\n<PRE>\n", fullpath);
   fprintf(to_write, "Item\r\n");
   fprintf(to_write, "<HR>\r\n");
//   <A HREF="ITEM_NAME">LINK_TO_ITEM</A>
	fprintf(to_write, "<A HREF=\"..\">..</A>\r\n");
	while((item = readdir(directory)))
	{
		strcpy(buf, fullpath);
		strcat(buf, "/");
		strcat(buf, item->d_name);
		
		if(stat(buf, &item_info) >= 0)
		{
			char* slash = S_ISDIR(item_info.st_mode) ? "/" : "";
			fprintf(to_write, "<A HREF=\"%s%s\">%s%s</A>\r\n", item->d_name, slash, buf, slash);
		}
	}
	fprintf(to_write, "</PRE>\r\n<HR>\r\n<ADDRESS>%s</ADDRESS>\r\n</BODY></HTML>\r\n", SERVER_NAME);
	closedir(directory);
	return 0;
}
int perform_get(FILE* to_write, char* path, char* protocol)
{
	char* current_dir = getenv("PWD"); //Это текущая директория
	char* fullpath = (char*)malloc(MAX_URL_SIZE*sizeof(char));//Полный путь к запр. ресурсу
		 
//Склеиваем полный путь к запрашиваемому ресурсу
	strcat(fullpath,current_dir);
	strcat(fullpath,path);
		
	struct stat fileinfo;
//Проверка на существование
	if (stat(fullpath, &fileinfo) < 0)
	{
		throw_error(to_write, 404, "Requested resourse not found.");
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
			return result;
		}
		else 	//значит файл index.html не существует, выведем список содержимого директории
		{
			int result = show_directory(to_write, fullpath);
			return result;
		}
	}
	if (S_ISREG(fileinfo.st_mode))
	{
		int result = throw_file(to_write, fullpath, fileinfo);
		return result;
	}
	return -1;
}
int parse(FILE* to_write, char* request)
{
/*Выделение метода, uri, протокола*/
	char *method;
	char *path;

	if (!request || strlen(request) < 3) return -1;
	printf("QUERY: %s", request);

	method = strtok(request, " ");
	path = strtok(NULL, " ");
	protocol = strtok(NULL, "\r");
	if (!method || !path || !protocol) return -1;
/*Выделение метода, uri, протокола*/

	if(strcasecmp("GET", method))
		perform_get(to_write, path, protocol);
	else 
	if(strcasecmp("POST", method))
	{	}
	else
   {
		throw_error(to_write, 501, "Unknown method.");
		return 501;
	}
	return 0;
}

int main()
{
	return 0;
}
