#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>

#define SIGNATURE_PADRE 0x5555
#define SIGNATURE_HIJO 0xAAAA

typedef struct {
	sem_t sem;
	int value;
} Signatures_t;


int main(int argc, char const *argv[]){

	int fd = shm_open("/my_shm", O_RDWR | O_CREAT, 0600);
	assert(fd != -1);

	ftruncate(fd, 1<<12);

	Signatures_t *p = mmap(NULL, sizeof(Signatures_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if( p == MAP_FAILED){
		perror("Error mmap: ");
	}
	assert(p != MAP_FAILED);

	if(argc > 1){
		if(sem_init(&p->sem, 1, 0) != 0){
			perror("sem_init");
			return -1;
		}
	}

	for(unsigned long i = 0; i < 100000000; ++i){
		sem_post(&p->sem);
		// p->value--;	
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