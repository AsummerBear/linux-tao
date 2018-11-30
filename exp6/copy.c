#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFFER_SIZE 1024
#define OFFSET 10240

int main(int argc, char** argv)
{
	int src_file,dest_file;
	unsigned char buff[BUFFER_SIZE];
	int real_read_len;

    src_file = open(argv[1],O_RDONLY);
	dest_file = open(argv[2],O_WRONLY|O_CREAT,644);
	if(src_file < 0 || dest_file < 0)
	{
		printf("Open file error!\n");
		exit(1);
	}

    lseek(src_file,-OFFSET,SEEK_END);

    while((real_read_len = read(src_file,buff,sizeof(buff))) > 0)
    {
    	write(dest_file,buff,real_read_len);
    }
	chmod(argv[2], strtoul(argv[3], NULL,8));
    close(dest_file);
    close(src_file);

    return 0;
}
