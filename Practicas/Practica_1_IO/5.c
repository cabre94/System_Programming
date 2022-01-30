#define _LARGEFILE64_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> /* exit */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <string.h>


const int NUM_BLOCKS = 10;

int main(){

	int fds[4] = {0};
	const char *f_names[4] = {"f_32.txt", "f_512.txt", "f_8kb.txt", "f_8mb.txt"};
	const off_t holes[4] = {32, 512, 8 * 1024, 1024 * 1024};
	// const off_t holes[4] = {32, 512, 8192, 1048576};
	const char *block = "abc\n";

	// Creo los archivos
	for (int i = 0; i < 4; ++i){

		fds[i] = open(f_names[i], O_CREAT | O_TRUNC | O_RDWR, 0666);
		if (fds[i] == -1){
			perror("open");
			exit(EXIT_FAILURE);
		}

		// Escribo un bloque
		if (write(fds[i], block, strlen(block)) == -1){
			perror("Block write");
			exit(1);
		}

		for (int j = 0; j < NUM_BLOCKS - 1; ++j){
			// Hueco
			if (lseek64(fds[i], holes[i], SEEK_CUR) < 0){
				perror("lseek64");
				exit(1);
			}
			// Escribo bloque
			if (write(fds[i], block, strlen(block)) == -1){
				perror("Block write");
				exit(1);
			}
		}
	}

	// Saco informacion con fstat;
	struct stat st;

	for (int i = 0; i < 4; ++i){
		if (fstat(fds[i], &st) == -1){
			perror("Stat");
			exit(1);
		}

		printf("st_size: %ld\n", st.st_size);
		printf("st_blocks: %ld\n", st.st_blocks);
		// printf("st_blksize: %ld\n", st.st_blksize);
		printf("\n");
	}

	for (int i = 0; i < 4; ++i){
		close(fds[i]);
	}

	return 0;
}