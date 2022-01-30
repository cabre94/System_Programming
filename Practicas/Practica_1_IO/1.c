#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>		/* exit */
#include <fcntl.h>
#include <assert.h>
#include <errno.h> 
#include <string.h>


int main(){

	int fd = open("practica_1.txt", O_CREAT | O_EXCL | O_RDWR, 0666);

	if(fd < 0){
		perror("Fallo en practica.txt");
		exit(1);
	}

	const char* str = "Holistic file\n";

	write(fd, str, strlen(str));

	close(fd);


	return 0;
}