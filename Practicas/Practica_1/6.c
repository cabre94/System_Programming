#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> /* exit */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <string.h>


int main(){

	const char* f_name = "kk.txt";
	const char* h_link = "kk_hardlink.txt";
	const char* s_link = "kk_symlink.txt";

	int fd = open("kk.txt", O_RDWR | O_CREAT | O_TRUNC, 0777);
	if(fd == -1){
		perror("Open");
		exit(EXIT_FAILURE);
	}
	const char* str = "abc\n";
	write(fd, str, strlen(str));

	// Saco la metadata del archivo 
	struct stat st;

	if (fstat(fd, &st) == -1){
        perror("After creating stat");
        exit(EXIT_FAILURE);
    }
	printf("Al crear el file: %ld links\n", st.st_nlink);

	// Creo un hardlink
	if(link(f_name, h_link) == -1){
		perror("Hardlink");
		exit(EXIT_FAILURE);
	}

	// Saco la metadata del archivo 
	if (fstat(fd, &st) == -1){
        perror("Stat after Hardlink");
        exit(EXIT_FAILURE);
    }
	printf("Despues del hardlink: %ld links\n", st.st_nlink);
	
	// Creo un link simbolico
	if(symlink(f_name, s_link) == -1){
		perror("Symlink");
		exit(EXIT_FAILURE);
	}

	// Saco la metadata del archivo 
	if (fstat(fd, &st) == -1){
        perror("Stat after Symlink");
        exit(EXIT_FAILURE);
    }
	printf("Despues del symlink: %ld links\n", st.st_nlink);


	return 0;
}