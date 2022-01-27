#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>		/* exit */
#include <fcntl.h>
#include <assert.h>
#include <errno.h> 
#include <string.h>
#include <sys/types.h>	/* umask */
#include <sys/stat.h>	/* umask */


int main(){

	/* Seteo la mascara*/
	umask(0002);	/*the previous value of the mask is returned*/

	/* Full access */
	// int fd = open("access_7.txt", O_CREAT /*| O_EXCL*/ | O_RDWR, 0777);
	int fd = open("access_7.txt", O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRGRP); //0740
	if(fd < 0){
		perror("Fallo en access_7.txt");
		exit(1);
	}

	const char* str7 = "7\n";
	write(fd, str7, strlen(str7));
	close(fd);

	/* Permiso de Lectura */
	fd = open("access_4.txt", O_CREAT | O_EXCL | O_RDWR, 0774);
	if(fd < 0){
		perror("Fallo en access_4.txt");
		exit(1);
	}

	const char* str4 = "4\n";
	write(fd, str4, strlen(str4));
	close(fd);

	/* Permiso de Escritura */
	fd = open("access_2.txt", O_CREAT | O_EXCL | O_RDWR, 0773);
	if(fd < 0){
		perror("Fallo en access_2.txt");
		exit(1);
	}

	const char* str2 = "2\n";
	write(fd, str2, strlen(str2));
	close(fd);

	

	return 0;
}