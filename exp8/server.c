#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define P sem_wait
#define V sem_post
#define mutex &mutex_real
#define BUFSIZE 200

int sockfd;//服务器socket
int fds[100];//客户端的socketfd,100个元素，fds[0]~fds[99]
int size =100 ;//用来控制进入客户端个数
char* IP = "0.0.0.0";
short PORT = 10222;
typedef struct sockaddr SA;
int pipe_fd;
sem_t mutex_real;       

//初始化socket
void init(){

    //创建socket获得描述符
    sockfd = socket(PF_INET,SOCK_STREAM,0);
    if (sockfd == -1){
        perror("创建socket失败");
        exit(-1);
    }

    //下面两行设置 socket 可以重用地址，这样在使用 Ctrl + C 终止 server 后不用等待即可再次创建 server
    int mw_optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&mw_optval,sizeof(mw_optval));
    
    //设置连接的参数，包括地址，端口，连接类型
    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);

    //绑定地址
    if (bind(sockfd,(SA*)&addr,sizeof(addr)) == -1){
        perror("绑定失败");
        exit(-1);
    }

    //监听端口
    if (listen(sockfd,100) == -1){
        perror("设置监听失败");
        exit(-1);
    }
}

//每一次连接会创建一个这样子的线程进行处理
void* service_thread(void* p){

    //获得连接套接字的描述符
    int fd = *(int*)p;
    int readbytes;
    while(1){

        char rqst[BUFSIZ];         //从socket接收到的字符串
        char resp[BUFSIZ];         //从管道文件读出来的字符串
        
        //接收消息，如果连接断开就说明退出了，释放描述符在数组中的位置
        if (recv(fd,rqst,BUFSIZ,0) <= 0){
            int i;
            for (i = 0;i < size;i++){
                if (fd == fds[i]){
                    fds[i] = 0;
                    break;
                }
            }
            printf("fd = %d\t连接断开\n",fd);
            pthread_exit(0);
        }

        //对管道的操作是互斥的，管道文件只有一对，所以每次只能有一个线程跟它们交互
        P(mutex);

        pipe_fd = open("myfifo", O_WRONLY);     //只写模式打开管道文件
        write(pipe_fd,rqst, BUFSIZ);           //把接收到的字符串写到管道文件
        close(pipe_fd);                          //关闭管道文件

        pipe_fd = open("myfifo", O_RDONLY);         //只读模式打开管道文件
        readbytes = read(pipe_fd, resp, BUFSIZ);    //从管道文件中读取内容
        close(pipe_fd);                        

        V(mutex);

        //把服务器接受到的信息发回给该客户端
        resp[readbytes] = '\0';
        send(fd,resp,strlen(resp),0);
    }
}

void service(){
    printf("服务器启动\n");

    //初始化互斥信号量
    sem_init(mutex, 0, 1);

    while(1){
        struct sockaddr_in fromaddr;
        socklen_t len = sizeof(fromaddr);

        //接受连接
        int fd = accept(sockfd,(SA*)&fromaddr,&len);
        if (fd == -1){
            printf("客户端连接出错...\n");
            continue;
        }

        //从数组中找到一个没被占用的位置，把连接套接字的描述符存进去
        int i = 0;
        for (i = 0;i < size;i++){
            if (fds[i] == 0){

                //记录客户端的socket
                fds[i] = fd;
                printf("接受一个连接：fd = %d\n",fd);

                //有客户端连接之后，启动线程给此客户服务
                pthread_t tid;
                pthread_create(&tid,0,service_thread,&fd);
                break;
            }
        }

        //如果数组100个位置都被占用，说明此时已经存在100个用户
        if (size == i){
            char* str = "已经达到100个用户连接上限!";
            send(fd,str,strlen(str),0); 
            close(fd);
        }
    }
}

int main(void){
    init();
    service();
    return 0;
}

