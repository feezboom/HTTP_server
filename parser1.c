void parser(char*request){
	char method[5];
	strncpy(method, request, 4);
	method[4] = '\0';
	int flag_meth;
	if(!strcmp(method, "GET ")){
		flag_meth = 1;
		char* add = get_adr(request+3);


	}
	else if !strcmp(method, "POST")){
		flag_meth = 2;
		char* add = get_adr(request+4);
	}
	else return NULL;
	//перенести current_dir  в глобальные переменные
	char* path = (char*)malloc((strlen(current_dir)+1)*sizeof(char));
    strcpy(path,current_dir);
	path=(char*)realloc(path,(strlen(path)+strlen(add)+1)*sizeof(char));
    strcat(path,add);
	
	//
	//
	//перенести в GET
	/*if((strlen(add) == 1) && (add[0] == '/') && (flag_meth = 1) {
        char index[]="/index.html";
        path=(char*)realloc(path,(strlen(path)+12)*sizeof(char));
        strcat(path,index);
        return path;
    }
    if(strstr(path,"?")!=NULL)
        *strstr(path,"?")='\0';*/
	// перенести в GET
	//
	//
    
	
	/* В GET
	int current_pos=strlen(path)-1;
    //поиск расширения файла
	while(path[current_pos]!='/') {
		if(path[current_pos]=='.')
			break;
		current_pos--;
	}
	char* content_type = NULL;
    char* file_extension = NULL;
	if(path[current_pos]=='/')
            content_type="application/octet-stream";
        else {
            file_extension=(char*)calloc(strlen(path+current_pos+1)+1,sizeof(char));
            strcpy(file_extension,path+current_pos+1);
            printf("%s\n",file_extension);
            if(strcmp(file_extension,"html")==0||strcmp(file_extension,"htm")==0)
                content_type="text/html";
            else if(strcmp(file_extension,"zip")==0)
                content_type="application/zip";
            else if(strcmp(file_extension,"torrent")==0)
                content_type="application/x-bittorrent";
            else if(strcmp(file_extension,"mp4")==0)
                content_type="video/mp4";
            else if(strcmp(file_extension,"mp3")==0)
                content_type="audio/mp3";
            else if(strcmp(file_extension,"wma")==0)
                content_type="audio/x-ms-wma";
            else if(strcmp(file_extension,"wav")==0)
                content_type="audio/vnd-wave";
            else if(strcmp(file_extension,"jpg")==0||strcmp(file_extension,"jpeg")==0)
                content_type="image/jpeg";
            else if(strcmp(file_extension,"json")==0)
                content_type="application/json";
            else if(strcmp(file_extension,"js")==0)
                content_type="application/javascript";
            else if(strcmp(file_extension,"ogg")==0)
                content_type="audio/ogg";
            else if(strcmp(file_extension,"pdf")==0)
                content_type="application/pdf";
            else if(strcmp(file_extension,"mpeg")==0)
                content_type="video/mpeg";
            else if(strcmp(file_extension,"flv")==0)
                content_type="video/x-flv";
            else if(strcmp(file_extension,"txt")==0||strcmp(file_extension,"log")==0)
                content_type="test/plain";
            else if(strcmp(file_extension,"xml")==0)
                content_type="text/xml";
            else if(strcmp(file_extension,"css")==0)
                content_type="text/css";
            else if(strcmp(file_extension,"csv")==0)
                content_type="text/csv";
            else if(strcmp(file_extension,"png")==0)
                content_type="image/png";
            else if(strcmp(file_extension,"gif")==0)
                content_type="image/gif";
            else if(strcmp(file_extension,"svg")==0)
                content_type="image/svg+xml";
            else if(strcmp(file_extension,"tiff")==0)
                content_type="image/tiff";

            else
                content_type="application/octet-stream";

				*/
}

char* get_adr(char* buf)
{
	char *p1 = buf+1;
	char *p2 = strstr(buf+1, " HTTP/1.1");
	if(!p2)
	{
		return NULL;
	}
	unsigned len = strlen(p1) - strlen(p2);
	char* adr = malloc(len+1);
	if(!adr)
	{
		return NULL;
	}
	strncpy(adr, p1, len);
	//adr[len] = '\0';
	return adr;
}