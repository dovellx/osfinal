#include "stdio.h"
#include "string.h"
#include "type.h"

void inject(char *elf_file) {
	char shellcode[] = {0xc7,0x45,0xf4,0x01};
	int fd = open(elf_file,O_RDWR);
	u8 buffer[20480];
	read(fd, buffer, 0x1011);
	write(fd,shellcode,sizeof(shellcode));
	close(fd);
	printf("Successfully inject the target file\n");
}

int main(int argc, char** argv){
	if(argc!=2){
		printf("Input the elf_file name\n");
		return 0;
	}
	inject(argv[1]);
	return 0;
}
