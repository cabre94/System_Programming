#define _LARGEFILE64_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> /* exit */
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>


int main(){

	int fd = open("file4.txt", O_CREAT | O_TRUNC | O_RDWR, 0666);
	if(fd < 0){
		perror("Fallo en practica.txt");
		exit(1);
	}

	// EScribo algo al principio del file
	const char* str1 = "abc\n";
	if (write(fd, str1, strlen(str1)) == -1){
        perror("Start write");
        exit(1);
    }

	// Corro el offset
	off_t empty_spc = 4096;
	if (lseek64(fd, empty_spc, SEEK_CUR) < 0){
        perror("lseek64");
        exit(1);
    }

	// Escribo de nuevo
	const char* str2 = "\nxyz";
	if (write(fd, str2, strlen(str2)) == -1){
        perror("End write");
        exit(1);
    }



	printf("Suma de stlen: %ld\n", strlen(str1)+strlen(str2));

	// Saco la informacion con fstat
	struct stat st;

	if (fstat(fd, &st) == -1){
        perror("Stat");
        exit(1);
    }

	printf("Size con fstat: %ld\n", st.st_size);


	close(fd);

	return 0;
}