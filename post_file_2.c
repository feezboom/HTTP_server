#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#define REWRITE 1 //if 1 - rewrite, if 0 - refuse

char* current_dir;
int post_file(char*filename, char* buf){
	char* add = "/clients_files";
	char*client_dir = (char*)malloc(sizeof(char)*(strlen(current_dir)+1));
	strcpy(client_dir, current_dir);
	client_dir = (char*)realloc(client_dir, sizeof(char)*(strlen(client_dir)+14));
	strcat(client_dir, add);
	char*client_file = (char*)malloc(sizeof(char)*(strlen(current_dir)+1));
	strcpy(client_file, client_dir);
	client_file = (char*)realloc(client_file, sizeof(char)*(strlen(client_dir)+1));
	strcat(client_file, "/");
	client_file = (char*)realloc(client_file, sizeof(char)*(strlen(client_dir)+strlen(filename)+1));	
	strcat(client_file, filename);
	struct stat fileinfo;
	
	if (stat(client_dir, &fileinfo) <0){
		if(mkdir(client_dir, 0777) == -1){
			//error
		}
		FILE* new_file = fopen(client_file, "wb");
		fwrite(buf, sizeof(char), strlen(buf)*sizeof(char), new_file);
		fclose(new_file); 
	} else{
		
		struct stat fileinfo1;
		if (stat(client_file, &fileinfo1) < 0){
			printf("File doesn't exist\n");
			FILE* new_file = fopen(client_file, "wb");
			fwrite(buf, sizeof(char), strlen(buf)*sizeof(char), new_file);
			fclose(new_file); 
		} else {
			printf("File exists\n");
			if (REWRITE == 1){
				FILE* new_file = fopen(client_file, "wb");
				fwrite(buf, sizeof(char), strlen(buf)*sizeof(char), new_file);
				fclose(new_file); 
			} else{
				//error
			}
		}
	}
	free(client_dir);
	free(client_file);
	return 0;
}



int main(int argc, char** argv){
	//current_dir = argv[1];
	post_file("a1.txt", "qqq");
	//post_file("a1.txt", "ccc");
	return 0;
}
