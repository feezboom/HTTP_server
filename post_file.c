#include<fcntl.h>
#include<dirent.h>
#include<string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include<stdio.h>

char* current_dir;
int post_file(char*filename, char* buf){
	char* add = "/clients_files";
	char*client_dir = (char*)malloc(sizeof(char)*(strlen(current_dir)+1));
	strcpy(client_dir, current_dir);
	client_dir = (char*)realloc(client_dir, sizeof(char)*(strlen(client_dir)+14));
	strcat(client_dir, add);
	
	
	DIR* cd = opendir(client_dir);
	if (cd == NULL){
		if(mkdir(client_dir, 0777) == -1){
			//error
		}
		cd = opendir(client_dir);
	} else{
		printf("Yeah\n");
		char*client_file = (char*)malloc(sizeof(char)*(strlen(current_dir)+1));
		strcpy(client_file, client_dir);
		client_file = (char*)realloc(client_file, sizeof(char)*(strlen(client_dir)+1));
		strcat(client_file, "/");
		client_file = (char*)realloc(client_file, sizeof(char)*(strlen(client_dir)+strlen(filename)+1));
		//client_file[strlen(client_dir)] = "/";
		//client_file[strlen(client_dir)+1] = "\0";	
		strcat(client_file, filename);
		if (open(client_file, O_CREAT|O_EXCL) == -1){
			printf("File exist");
		}
		FILE* new_file = fopen(client_file, "wb");
		fwrite(buf, sizeof(char), strlen(buf)*sizeof(char), new_file);
		fclose(new_file); 
	}
}


int main(int argc, char** argv){
	current_dir = argv[1];
	post_file("a1.txt", "qqq");
	return 0;
}
