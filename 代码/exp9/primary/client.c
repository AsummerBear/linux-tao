#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#define MAXSIZE 100
 
int main(int argc,char *argv[])
{
	int pfd;
	char comfifo[MAXSIZE];
	char msg[MAXSIZE];
	if(argc < 2)
    {
		printf("argument error!");
		exit(1);
	}
	if( (pfd = open(argv[1],O_WRONLY)) < 0)   //服务端公共管道
    {
		perror("fail to open comman fifo file");
		exit(1);
	}
	while(1)
    {
        printf("输入发送内容：");
		fgets(msg,sizeof(msg),stdin);   //输入发送内容
		sprintf(comfifo,"%d %s",getpid(),msg);
		printf("send to server :%s\n",comfifo);
		write(pfd,comfifo,strlen(comfifo) + 1); //写入公共管道
	}
    exit(0);
}

