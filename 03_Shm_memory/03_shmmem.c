#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>

#define SIGNATURE_PADRE 0x5555
#define SIGNATURE_HIJO 0xAAAA

typedef struct {
	int value;
} Signatures_t;


int main(int argc, char const *argv[]){

	int flags = O_RDWR;
	int create = 0;

	if(argc > 1){
		flags |= O_CREAT;
		create = 1;
	}

	int fd = shm_open("/my_shm", flags, 0600);
	assert(fd != -1);

	if(create){
		ftruncate(fd, 1<<12);
	}

	Signatures_t *p = mmap(NULL, sizeof(Signatures_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if( p == MAP_FAILED){
		perror("Error mmap: ");
	}
	assert(p != MAP_FAILED);

	for(unsigned long i = 0; i < 1000000000; ++i){
		// printf("value antes: %d\n", p->value);
		p->value++;	
		// printf("value despues: %d\n", p->value);
	}

	munmap(p, sizeof(Signatures_t));

	return 0;
}

/*
MAP_ANONYMOUS: parecido a un malloc, no necesita un file descriptor. No esta asociada a ningun nombre

Aca no hay data race porque los dos procesos fueron a escribir a dos direcciones de memoria distinta

Aca el fork esta hecho despues del mmap, asi que los dos proceos tienen las misma direccion de memoria, 
pero no tiene porque ser asi.
*/