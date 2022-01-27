#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int main(){

	int fd = open("kk", O_CREAT | O_TRUNC | O_RDWR, 0666);
	
	// int fd2 = open("kk2", O_CREAT | O_TRUNC | O_RDWR, 0666);
	int fd2 = 16;

	if(dup2(fd , fd2) == -1){
		perror("Algo paso: ");
	}
	// if(dup2(fd2 , fd) == -1);	// Error. No es un fd

	printf("%d\t%d\n", fd, fd2);

	if(close(fd2) == -1){
		perror("Error close fd2");
		if(errno == EBADF){
			printf("fd: EBADF\n");
		}
	}
	if(close(fd) == -1){
		perror("Error close fd");
		if(errno == EBADF){
			printf("fd: EBADF\n");
		}
	}



	return 0;
}