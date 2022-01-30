#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/wait.h>

typedef struct{
    sem_t mtx_sync;			// sinc de acceso, mutex
    sem_t sem_aval;			// cant. de entradas libres
    sem_t sem_used;			// bloqueo para proc. productores
    // int elem[pQ->Q_SZ];
    int *elem;
    unsigned int idx_put;
    unsigned int idx_get;
	int Q_SZ;
} Queue_t;

// Inicializa (debe residir en un segmento de shared memory)
void QueueInit(Queue_t *pQ);
// Desstruye el contenedor, liberando recursos
void QueueDestroy(Queue_t *pQ);
// Agrega un Nuevo elemento. Bloquea si no hay espacio
void QueuePut(Queue_t *pQ, int elem);
// Remueve y retorna un elemento, bloquea si no hay elementos
int QueueGet(Queue_t *pQ);
// recupera la cantidad de elementos en la cola
int QueueSize(Queue_t *pQ);
// monitorea la cola
void QueueMonitor(Queue_t *pQ);


int main(int argc, const char *argv[]){
	
    // hago el mapeo al vector pQ
    Queue_t *pQ = mmap(NULL, sizeof(Queue_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    //inicializo la la cola
    QueueInit(pQ);

    int pid, i;

    pid = fork();
    switch (pid){
		case -1:
			perror("fork child 1");
		case 0:
			//child - agrega elementos
			for (i = 0; i < 100; i++){
				QueuePut(pQ, i);
			}
			break;
		default:	//father
			for (i = 0; i < 100; i++){
				if ((i % 10) == 0){
					QueueMonitor(pQ);
				}
				QueueGet(pQ);
			}
    		wait(NULL);
		    printf("Terminaron todos los procesos\n");
			
    		assert(QueueSize(pQ) == 0);

			QueueDestroy(pQ);
    		munmap(pQ, sizeof(Queue_t));
    }
	
    return 0;
}






// Inicializa (debe residir en un segmento de shared memory)
void QueueInit(Queue_t *pQ){
	pQ->Q_SZ = 10; 
	pQ->idx_put = 0;
	pQ->idx_get = 0;

	pQ->elem = (int *) malloc(pQ->Q_SZ*sizeof(int));

	// Me canse del perror
	assert(sem_init(&pQ->mtx_sync, 1, 1) 	!= -1);
	assert(sem_init(&pQ->sem_aval, 1, pQ->Q_SZ) != -1);
	assert(sem_init(&pQ->sem_used, 1, 0) 	!= -1);
}

// Desstruye el contenedor, liberando recursos
void QueueDestroy(Queue_t *pQ){
	assert(sem_destroy(&pQ->mtx_sync) != -1);
	assert(sem_destroy(&pQ->sem_aval) != -1);
	assert(sem_destroy(&pQ->sem_used) != -1);
	free(pQ->elem);
}

// Agrega un Nuevo elemento. Bloquea si no hay espacio
void QueuePut(Queue_t *pQ, int elem){
	assert(sem_wait(&pQ->sem_aval) != -1);
	assert(sem_wait(&pQ->mtx_sync) != -1); // Por si alguien lo esta  usando

	// Hay lugar y nadie lo esta usando
	pQ->elem[pQ->idx_put % pQ->Q_SZ] = elem;
	pQ->idx_put++;

	assert(sem_post(&pQ->mtx_sync) != -1); // Lo libero
	assert(sem_post(&pQ->sem_used) != -1);
}

// Remueve y retorna un elemento, bloquea si no hay elementos
int QueueGet(Queue_t *pQ){
	assert(sem_wait(&pQ->sem_used) != -1);
	assert(sem_wait(&pQ->mtx_sync) != -1); // Por si alguien lo esta  usando

	// Hay elementos y nadie lo esta usando
	int value = pQ->elem[pQ->idx_get % pQ->Q_SZ];
	pQ->idx_get++;

	assert(sem_post(&pQ->mtx_sync) != -1); // Lo libero
	assert(sem_post(&pQ->sem_aval) != -1);
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
        printf("%d -> ", pQ->elem[(pQ->idx_get + i) % pQ->Q_SZ]);
    }
	putchar('\n');

	assert(sem_post(&pQ->mtx_sync) != -1); // Lo libero

}