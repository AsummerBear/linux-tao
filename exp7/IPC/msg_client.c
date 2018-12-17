#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ID 0x111

#define MSGSND 100
#define MSGRCV 200

typedef struct Msg{
    long type;
    char content[256];
}Msg;

int main(int argc, char *argv[])
{
    Msg msg;
	key_t msg_key = ftok(argv[1],ID);  
	if(msg_key == -1)
		exit(1);
    key_t msg_id = msgget(msg_key,0); 	//获取服务端消息队列

    while(1)
    {
        msgrcv(msg_id, &msg, 256, MSGRCV, 0);  		//等待接收消息
        printf("\t\t\tServer:%s\n",msg.content);
        printf("Client:");
        scanf("%s",msg.content);
		msg.type = MSGSND;		//设置发送码
        if(strcmp(msg.content,"q") == 0)
			break;
        msgsnd(msg_id, &msg, strlen(msg.content)+1, IPC_NOWAIT); //发送消息
    }
    return 0;
}
