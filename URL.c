#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>	
#include <sys/stat.h>
#include <sys/types.h>


int open_close (int argc, char ** argv)
{
	int fd;
	mode_t mode = S_IRUSR | S_IWUSR;
	int flags = O_WRONLY | O_CREAT | O_EXCL;
	if (argc < 2)
	{
		fprintf (stderr, "openclose: Too few arguments\n");
		fprintf (stderr, "Usage: openclose <filename>\n");
		return(1);
	}

	fd = open (argv[1], flags, mode);
	if (fd < 0)
	{
		fprintf (stderr, "openclose: Cannot open file '%s'\n",
				argv[1]);
		return(1);
	}
	
	if (close (fd) != 0)
	{
		fprintf (stderr, "Cannot close file (descriptor=%d)\n", fd);
		return(1);
	}	
	exit (0);
}

int URLshit(char* pathto)
{
	int i;
	i=chdir(pathto);
	if(i==-1)
	{
		//GoERROR
		return(-1);
	}
	else
	{
		char* filename;
		int len_addr=strlen(pathto);
		for(i=len_addr; pathto[i]!='/';i--);
		filename= (char*) malloc(len_addr-i * sizeof(char)); 
		int j;
		for(j=i;j<len_addr;j++)
			filename[j-i]=pathto[j];
		i=getfile(filename);
	}
	return(0);
}
	

int getfile(char* filename)
{
	//char *args[4];
	//int result;

	//result = execvp(alladdr);
	char* type;
	int i;
	type = (char*) malloc(4 * sizeof(char));
	int j;
	for(j=0;j<4;j++)
		type[j]=filename[strlen(filename-4+j)];
	//DIR * thisdir;
	//thisdir = opendir(dirname);
	
	/*if(strcmp(type,".txt"))  
	{
		FILE * thisfile;
		thisfile=fopen(filename, "r+");
		return(thisfile);
		system(alladdr);
	}
	else
	if(strcmp(type,".exe"))	
	{
		system(filename);
	}
	return(0);*/
	
	execl(filename, " ", " ", " ",NULL);
	return(0);
};	


int main()
{

	
	return(0);
}	
