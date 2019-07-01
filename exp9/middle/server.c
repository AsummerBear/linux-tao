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
	int server_sockfd;              //服务器端socket
	int client_sockfd;              //客户端socket
	int len, sin_size, size;
	char msg[MSG_SIZE+1],file_name[MSG_SIZE+1];              
    char *buf;                      //数据传送的缓冲区
	struct sockaddr_in my_addr;     //服务器网络地址结构体
	struct sockaddr_in remote_addr; //客户端网络地址结构体
    FILE *fp = NULL;

	memset(&my_addr,0,sizeof(my_addr));         //数据初始化--清零
	my_addr.sin_family = AF_INET;               //设置为IP通信
	my_addr.sin_addr.s_addr = INADDR_ANY;       //服务器IP地址--允许连接到所有本地地址上
	my_addr.sin_port = htons(atoi(argv[1]));    //服务器端口号
	
	/*创建服务器端socket--IPv4协议，面向连接通信，TCP协议*/
	if((server_sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{  
		perror("socket error");
		exit(1);
	}
 
	/*将socket绑定到服务器的网络地址上*/
	if(bind(server_sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) < 0)
	{
		perror("bind error");
		exit(1);
	}
	
	/*监听连接请求--监听队列长度为5*/
	if(listen(server_sockfd, 5) < 0)
	{
		perror("listen error");
		exit(1);
	}
	
	sin_size = sizeof(struct sockaddr_in);

	/*等待客户端连接请求到达*/
	if((client_sockfd = accept(server_sockfd,(struct sockaddr *)&remote_addr,&sin_size)) < 0)
	{
		perror("accept error");
		exit(1);
	}
	printf("Client %s has connect.\n",inet_ntoa(remote_addr.sin_addr));
	send(client_sockfd,"Connect successfully!\n",22,0);

	/*创建传输文件*/
    len = recv(client_sockfd,msg,MSG_SIZE,0);
    msg[len] = '\0';
    strcpy(file_name,msg);
    if((fp = fopen(file_name, "w")) == NULL)
    {
        perror("creat file error");
        exit(1);
    }

    /*获取缓冲区大小*/
    len = recv(client_sockfd,msg,MSG_SIZE,0);
    msg[len] = '\0';
    size = atoi(msg);
    buf = (char *)malloc(sizeof(char)*size); 
	send(client_sockfd,"ok",2,0);		//发送给服务端开始传送数据的信号，防止msg缓冲区读入文件流

    /*数据流写入文件*/
    while((len = recv(client_sockfd, buf, size, 0)) > 0)
    {
        if(fwrite(buf, sizeof(char), len, fp) < len)
        {
            perror("file write failed");
            break;
        }
        bzero(buf, size);              //清空数据缓冲区
    }

    printf("Receive file:\t%s from client successful!\n", file_name);
    fclose(fp);              //关闭文件
	close(client_sockfd);   //关闭socket
	close(server_sockfd);
    
	exit(0);
}

