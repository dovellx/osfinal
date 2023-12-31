#include "stdio.h"

int main(int argc, char * argv[])
{
	int i;
	if(argc==1)
		return 0;
	struct stat stat1;
	stat(argv[1],&stat1);
	char buf[1024];
	int fd1 = open(argv[1],O_RDWR);
	read(fd1,buf,stat1.st_size);
	for(int i = 0; i < stat1.st_size; i++)
	{
		printf("%c",buf[i]);	
	}
	printf("\n");
	return 0;
}
