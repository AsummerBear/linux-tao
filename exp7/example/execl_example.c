#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main()
{
	pid_t pid;
	if((pid=fork())==-1)
		printf("fork error");
	else if(pid==0)
	{
		execl("/bin/ls","ls","-l","/home",(char *)0);
	}
	else
	{
		printf("farther process ok\n");
	}

	return 0;
}
