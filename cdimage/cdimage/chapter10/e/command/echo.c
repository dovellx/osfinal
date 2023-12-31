#include "stdio.h"
#include "string.h"

int main(int argc, char * argv[])
{
    	if(open(argv[1], O_RDWR)==-1)
	{
		printf("Choose an existed file\n");
		return -1;
	}
	int fd = open(argv[1], O_RDWR);
	char * buf = argv[2];
	write(fd, buf, strlen(buf));
	close(fd);
	return 0;
}
