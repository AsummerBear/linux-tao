#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>

#define MAXSIZE 100
 
struct MSG
{
	char id[20];
	char str[MAXSIZE];
};
 
void main(int argc,char *argv[])
{	
	struct MSG msg;
	int pfd;
	char comfifo[MAXSIZE];
	if(argc < 2)
    {
		printf("argument error!\n");
		exit(1);
	}
    if((access(argv[1],F_OK)) == -1)   
    {   
        if(mkfifo(argv[1],0666) < 0)
        {
		    perror("fail to comman fifo");
		    exit(1);
	    } 
    }
	if( (pfd = open(argv[1],O_RDONLY)) < 0)  //打开公共管道
    {
		perror("fail to open comman fifo");
		exit(1);
	}
	while(1)
    {
		read(pfd,comfifo,sizeof(comfifo)); //从公共管道读内容
		char *s = strtok(comfifo," ");
		strcpy(msg.id,s);
		s = strtok(NULL," ");
		strcpy(msg.str,s);
		printf("recv from client: %s  msg:%s\n",msg.id,msg.str);
	}
	exit(0);
}

