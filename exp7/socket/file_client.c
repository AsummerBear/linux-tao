#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MSG_SIZE 512

void main(int argc, char *argv[])
{
	int client_sockfd;
	int len, size, send_size = 0;
	struct sockaddr_in remote_addr; //服务器端网络地址结构体
	char msg[MSG_SIZE+1],file_name[MSG_SIZE+1]; 
   	char *buf;                      //数据传送的缓冲区
   	FILE *fp = NULL;

	memset(&remote_addr,0,sizeof(remote_addr));         //数据初始化--清零
	remote_addr.sin_family = AF_INET;                   //设置为IP通信
	remote_addr.sin_addr.s_addr = inet_addr(argv[1]);   //服务器IP地址
	remote_addr.sin_port = htons(atoi(argv[2]));        //服务器端口号
	
	/*创建客户端socket--IPv4协议，面向连接通信，TCP协议*/
	if((client_sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("socket error");
		exit(1);
	}
	
	/*将socket绑定到服务器的网络地址上*/
	if(connect(client_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr)) < 0)
	{
		perror("connect error");
		exit(1);
	}

	printf("connected to server\n");
	len = recv(client_sockfd,msg,MSG_SIZE,0);         //接收服务器端信息
    msg[len] = '\0';
	printf("%s",msg);                                 //打印服务器端信息

    /*输入传输文件名称*/
    printf("File name:");
    scanf("%s",file_name);
    if((fp = fopen(file_name, "r")) == NULL)
    {
        perror("open file error");
        exit(1);
    }
    send(client_sockfd,file_name,strlen(file_name),0);
    
    /*输入缓冲区大小*/
    printf("Buffer size:");
    scanf("%s",msg);
    size = atoi(msg);
    buf = (char *)malloc(sizeof(char)*size);     
    send(client_sockfd,msg,strlen(msg),0);
    if(recv(client_sockfd,msg,MSG_SIZE,0) > 0)         //接收服务器确认消息
	printf("Begin to send...\n");
	
    /*传输文件*/
    while((len = fread(buf, sizeof(char), size, fp)) > 0)    // 每读取一段数据，便将其发送给客户端，循环直到文件读完为止
    {
        if(send(client_sockfd, buf, len, 0) < 0)
        {
            perror("file send failed");
            break;
        }
	    send_size += len;
	    printf("%d bytes has been send.\n",send_size);
        bzero(buf, size);
    }
    printf("Send file:%s successful.\n",file_name);
	
	fclose(fp);              //关闭文件
	close(client_sockfd);   //关闭socket
	exit(0);
}

