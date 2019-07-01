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

int main(int argc, char *argv[])
{
    key_t key_id, sem_key, sems;

    key_id = ftok(argv[1], ID);
    key_t shm_id = shmget(key_id,0,0);  
    char *buf = (char *)shmat(shm_id, NULL, 0); //取得存放生产物品的共享内存

    sem_key = ftok(argv[1],ID1);
    sems = semget(sem_key,0,0);        //取得信号量集

    while(1)
    {
        P(sems,FULL);
        printf("Get:%s\n",buf);
        if(strcmp(buf,"no") == 0){
            V(sems,EMPTY);
            break;
        }
        V(sems,EMPTY);
    }
    return 0;
}

