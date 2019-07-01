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

int main(int argc, char *argv[])
{
    key_t key_id;
    key_id = ftok(argv[1], ID);
    key_t shm_id = shmget(key_id,0,0);  //取得服务器的信息量
    char *addr = (char *)shmat(shm_id, NULL, 0);

    key_t sem_key;
    sem_key = ftok(argv[1],ID1);

    key_t sem_id = semget(sem_key,0,0);  //取得服务器公用内存的key值

    struct sembuf p = {1, -1, 0};    //P操作：对1号信息量—1 与服务器的刚好相反
    struct sembuf v = {0, 1, 0};     //V操作：对0号信息量+1

    while(1)
    {
        semop(sem_id, &p, 1);
        printf("Ser:>%s\n",addr);
        printf("Cli:>");
        scanf("%s",addr);
        if(strcmp(addr,"quit") == 0){
            semop(sem_id, &v, 1);
            break;
        }
        semop(sem_id, &v, 1);
    }
    return 0;
}
