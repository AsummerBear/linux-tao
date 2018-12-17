#ifndef SEM_FUNC_H
#define SEM_FUNC_H
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
	struct seminfo *__buf;
};

void init_semaphore(key_t sid,int semnum,int val);
int P(key_t semid, int semnum);
int V(key_t semid, int semnum);

#endif
