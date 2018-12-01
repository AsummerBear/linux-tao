#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main()
{
	pid_t pid;
	if((pid=fork())==-1)
		perror("fork");
	else if(pid==0)
	{
		printf("I am child process\n");
		printf("pid=%d,ppid=%d\n",getpid(),getppid());
		sleep(5);
		printf("pid=%d,ppid=%d\n",getpid(),getppid());
		printf("child process exited\n");
	}
	else
	{
		printf("I am farther process\n");
		sleep(1);
		printf("farther process exited\n");
	}

	exit(0);
}
