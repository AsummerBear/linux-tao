#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "sem_func.h"

//初始化信号量
void init_semaphore(key_t sid,int semnum,int val)
{
    union semun semopts;
    semopts.val = val;
    semctl(sid,semnum,SETVAL,semopts);
}

//P操作
int P(key_t semid, int semnum)    
{
	struct sembuf semOp ;
	semOp.sem_num = semnum;
	semOp.sem_op  = -1;
	semOp.sem_flg = SEM_UNDO;
	return semop(semid, &semOp, 1);
}
 
//V操作
int V(key_t semid, int semnum)
{
	struct sembuf semOp ;
	semOp.sem_num = semnum;
	semOp.sem_op  = 1;
	semOp.sem_flg = SEM_UNDO;
	return semop(semid, &semOp, 1);
}
