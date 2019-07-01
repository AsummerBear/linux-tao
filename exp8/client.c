#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#define BUFSIZE 200

int sockfd;//客户端socket
char * IP = "0.0.0.0";//服务器的IP
short PORT = 10222;//服务器服务端口
typedef struct sockaddr SA;

//初始化
void init() {
	//创建
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr;

	//设置参数
	addr.sin_family = PF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr(IP);

	//进行连接
	if (connect(sockfd, (SA*)&addr, sizeof(addr)) == -1) {
		perror("无法连接到服务器");
		exit(-1);
	}
	printf("客户端启动成功\n*输入1可以查看服务端当前目录信息\n*输入2可以查看当前服务端上的时间\n*输入3可以查看服务端的系统版本\n");
}

void start() {
	pthread_t id;
	
	//创建一个线程进行接收
	void* recv_thread(void*);
	pthread_create(&id, 0, recv_thread, 0);
	char buf[BUFSIZ];
	//将用户的输入发给服务端
	while (1) {
		
		scanf("%s", buf);
		send(sockfd, buf, strlen(buf), 0);

	}
	close(sockfd);
}

void* recv_thread(void* p) {
	int numbytes;
	char buf[BUFSIZ];
	while((numbytes = recv(sockfd, buf, BUFSIZ, 0)) >0)
	{
		buf[numbytes] = '\0';
		printf("%s\n", buf);
	}
}

int main(void) {
	init();
	start();
	return 0;
}

