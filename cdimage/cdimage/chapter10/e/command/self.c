#include "stdio.h"

void print(){
	printf("Stack overflow");
	return 0;
}
void shellcode(char * buf, int size){
	char payload[4];
	int i = 0;
	for(i = 0; i < size; i++){
		payload[i] = buf[i]-'0';	
	}
	return ;
}
int main(int argc, char * argv[]){
	if(argc==1) printf("Please input txt file");
	int fd = open(argv[1], O_RDWR);
	char buffer[16];
	read(fd, buffer, 16);
	//printf("%s",buffer);
	shellcode(buffer,sizeof(buffer));
	return 0;
}



