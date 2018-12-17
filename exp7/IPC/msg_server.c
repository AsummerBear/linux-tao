#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ID 0x111

#define MSGSND 200
#define MSGRCV 100

typedef struct Msg{
    long type;
    char content[256];
}Msg;

int main(int argc, char *argv[])
{
	Msg msg;
 	key_t msg_key,msg_id;
    msg_key = ftok(argv[1],ID);
	if(msg_key == -1)
		exit(1);
	msg_id = msgget(msg_key,IPC_CREAT|IPC_EXCL|0666); //创建消息队列

    while(1)
	{
        printf("Server:");
        scanf("%s",msg.content);
		msg.type = MSGSND; 	//设置发送码
        msgsnd(msg_id, &msg, strlen(msg.content)+1,IPC_NOWAIT); //发送消息
        if(strcmp(msg.content,"q") == 0)
		{
            msgctl(msg_id,IPC_RMID,NULL);		//关闭消息队列
            break;
		}
        msgrcv(msg_id, &msg, 256, MSGRCV, 0);					//等待接受
        printf("\t\t\tClient:%s\n",msg.content);
    }
    return 0;
}
