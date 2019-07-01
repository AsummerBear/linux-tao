#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include "sem_func.h"

#define ID 0xFF
#define ID1 0x111
#define EMPTY 0
#define FULL 1

int main(int argc,char *argv[])
{
    key_t key_id,shm_id,sem_key,sems;
    char *buf;

    key_id = ftok(argv[1],ID);   //建立IPC通讯时指定的唯一ID值                 
    if(key_id == -1)
        exit(1);

    shm_id = shmget(key_id, 1024, IPC_CREAT|IPC_EXCL|0666);     //创建一个共享内存对象返回标识id，用于存放生产物品
    buf = (char *)shmat(shm_id,NULL,0); //把共享内存区对象映射到调用进程的地址空间

    sem_key = ftok(argv[1],ID1);
    if(sem_key == -1)
        exit(1);

    sems = semget(sem_key, 2, IPC_CREAT|IPC_EXCL|0666);  //创建信号量集（0：empty；1：FULL)

    init_semaphore(sems,EMPTY,1);      //初始化empty信号量
    init_semaphore(sems,FULL,0);       //初始化full信号量

    while(1){
        P(sems,EMPTY);
        printf("Set:");
        scanf("%s",buf);               //生产物品存入共享内存
        if(strcmp(buf,"no")==0){
            V(sems,FULL);
            shmctl(shm_id,IPC_RMID,NULL);
            semctl(sems,EMPTY,IPC_RMID);
            semctl(sems,FULL,IPC_RMID);
            break;
        }
        V(sems,FULL);                  
    }
    return 0;
}

