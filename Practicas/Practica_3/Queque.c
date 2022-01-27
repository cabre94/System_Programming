#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/wait.h>

#include "Queque.h"

// Inicializa (debe residir en un segmento de shared memory)
void QueueInit(Queue_t *pQ){
	pQ->idx_put = 0;
	pQ->idx_get = 0;

	// Me canse del perror
	assert(sem_init(&pQ->mtx_sync, 1, 1) 	!= -1);
	assert(sem_init(&pQ->sem_aval, 1, Q_SZ) != -1);
	assert(sem_init(&pQ->sem_used, 1, 0) 	!= -1);
}

// Desstruye el contenedor, liberando recursos
void QueueDestroy(Queue_t *pQ){
	assert(sem_destroy(&pQ->mtx_sync) != -1);
	assert(sem_destroy(&pQ->sem_aval) != -1);
	assert(sem_destroy(&pQ->sem_used) != -1);
}

// Agrega un Nuevo elemento. Bloquea si no hay espacio
void QueuePut(Queue_t *pQ, int elem){
	assert(sem_wait(&pQ->sem_aval) != -1);
	assert(sem_wait(&pQ->mtx_sync) != -1); // Por si alguien lo esta  usando

	// Hay lugar y nadie lo esta usando
	pQ->elem[pQ->idx_put % Q_SZ] = elem;
	pQ->idx_put++;

	assert(sem_post(&pQ->mtx_sync) != -1); // Lo libero
	assert(sem_post(&pQ->sem_used) != -1);
}

// Remueve y retorna un elemento, bloquea si no hay elementos
int QueueGet(Queue_t *pQ){
	assert(sem_wait(&pQ->sem_used) != -1);
	assert(sem_wait(&pQ->mtx_sync) != -1); // Por si alguien lo esta  usando

	// Hay elementos y nadie lo esta usando
	int value = pQ->elem[pQ->idx_get % Q_SZ];
	pQ->idx_get++;

	assert(sem_post(&pQ->mtx_sync) != -1); // Lo libero
	assert(sem_post(&pQ->sem_aval) != -1);

	return value;
}

// recupera la cantidad de elementos en la cola
int QueueSize(Queue_t *pQ){
	assert(sem_wait(&pQ->mtx_sync) != -1); // Por si alguien lo esta  usando

	// Hay elementos y nadie lo esta usando
	int sz = pQ->idx_put - pQ->idx_get;

	assert(sem_post(&pQ->mtx_sync) != -1); // Lo libero

	return sz;
}

// monitorea la cola
void QueueMonitor(Queue_t *pQ){
	assert(sem_wait(&pQ->mtx_sync) != -1); // Por si alguien lo esta  usando

	// Hay elementos y nadie lo esta usando
	int sz = pQ->idx_put - pQ->idx_get;
	printf("-> ");
	for(int i = 0; i < sz; i++){
        printf("%d -> ", pQ->elem[(pQ->idx_get + i) % Q_SZ]);
    }
	putchar('\n');

	assert(sem_post(&pQ->mtx_sync) != -1); // Lo libero

}