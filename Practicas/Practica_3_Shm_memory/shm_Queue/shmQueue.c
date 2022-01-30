#include "shmQueue.h"
#include "Queque.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <errno.h>
#include <unistd.h>



Queue_t* create_shm(const char* memqueque_name){

	sem_t *sem_create = sem_open(memqueque_name, O_CREAT | O_EXCL | O_RDWR, 0666, 0);

	if(sem_create == SEM_FAILED){
		perror("Error en sem_open creator");
		return NULL;
	}

	int fd = shm_open(memqueque_name, O_CREAT | O_EXCL | O_RDWR, 0666);

	if(fd == -1){
		perror("Error en shm_open");
		sem_close(sem_create);
		return NULL;
	}

	int ret = ftruncate(fd, sizeof(Queue_t));

	if(ret == -1){
		perror("Error en ftruncate");
		sem_close(sem_create);
		return NULL;
	}
	
    // hago el mapeo al vector pQ
    Queue_t *pQ = mmap(NULL, sizeof(Queue_t), PROT_READ | PROT_WRITE, MAP_SHARED , fd, 0);
	close(fd);

	// Queue_t *pQ;
	if(pQ == MAP_FAILED){
		perror("Error en mmap");
		sem_close(sem_create);
		return NULL;
	}

	QueueInit(pQ);

	sem_post(sem_create);
	sem_close(sem_create);
	

	return pQ;
}


Queue_t* attach_shm(const char* memqueque_name){
	// hago el mapeo al vector pQ
    sem_t *sem_create = sem_open(memqueque_name, O_RDWR, 0666, 0);

	if(sem_create == SEM_FAILED){
		perror("Error en sem_open sem_create");
		return NULL;
	}

	int fd = shm_open(memqueque_name, O_RDWR, 0666);

	if(fd == -1){
		perror("Error en shm_open");
		sem_close(sem_create);
		return NULL;
	}


    Queue_t *pQ = mmap(NULL, sizeof(Queue_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);

	if(pQ == MAP_FAILED){
		perror("Error en mmap");
		sem_close(sem_create);
		return NULL;
	}

	sem_wait(sem_create);
	sem_post(sem_create);

	sem_close(sem_create);

	return pQ;
}

int detach_shm(Queue_t* pQ){
	return munmap(pQ, sizeof(Queue_t));
}



void destroy_shm(const char* memqueque_name){
	sem_unlink(memqueque_name);
	shm_unlink(memqueque_name);
}