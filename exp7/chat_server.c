#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#define ID 0xFF
#define ID1 0x111

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
	struct seminfo *__buf;
};

int main(int argc,char *argv[])
{
    key_t key_id;                //得到唯一的key_id值。
    key_id = ftok(argv[1],ID);   //ftok()上面有解析,ID=OxFF系统随机的唯一值                    
    if(key_id == -1){            //要是返回-1 ftok得到key_id失败
        exit(1);
    }
    printf("key_id = 0x%x\n",key_id);

    key_t shm_id;                //用唯一的key值申请信息量
    shm_id = shmget(key_id, 1024, IPC_CREAT|IPC_EXCL|0666);//8进制的666权限

    char *addr;                  
    addr = (char *)shmat(shm_id,NULL,0); //申请公用内存并与addr连接

    key_t sem_key;
    sem_key = ftok(argv[1],ID1);

    key_t sem_id = semget(sem_key, 2, IPC_CREAT|IPC_EXCL|0666);

        union semun init;
        init.val = 0;
        semctl(sem_id, 0, SETVAL, init);
        semctl(sem_id, 1, SETVAL, init);

        struct sembuf p = {0, -1, 0};   //p操作：对0号信息量-1 请与客户端的对比
        struct sembuf v = {1, 1 ,0};    //v操作：对1号信息量+1

    while(1){
        printf("Ser:>");
        scanf("%s",addr);               //输入信息存入内存
        if(strcmp(addr,"quit")==0){
            semop(sem_id,&v,1);
            shmctl(shm_id,IPC_RMID,NULL);
            semctl(sem_id,0,IPC_RMID);
            semctl(sem_id,1,IPC_RMID);
            break;
        }
        semop(sem_id, &v, 1);            //执行V操作，同步的关键！
        semop(sem_id, &p, 1);
        printf("Cli:>%s\n",addr);
    }
    return 0;
}
