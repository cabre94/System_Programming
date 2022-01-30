#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/wait.h>

#define Q_SZ 8

typedef struct{
    sem_t mtx_sync;			// sinc de acceso, mutex
    sem_t sem_aval;			// cant. de entradas libres
    sem_t sem_used;			// bloqueo para proc. productores
    int elem[Q_SZ];
    unsigned int idx_put;
    unsigned int idx_get;
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

// Funciones para los distintos procesos
void do_init(Queue_t *pQ, sem_t *sem_create){
    // pQ = mmap(NULL, sizeof(Queue_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	QueueInit(pQ);

	for(int i=0; i < 3; ++i){
		sem_post(sem_create);
	}
}

// void do_init(Queue_t **pQ){
// 	*pQ = mmap(NULL, sizeof(Queue_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
// 	QueueInit(*pQ);
// 	printf("Inicializado %p\n", *pQ);

// }

void do_put(Queue_t *pQ, sem_t *sem_create, sem_t *sem_destroy){
	sem_wait(sem_create);
	
	printf("Arranco la do_put\n");
	for (int i = 0; i < 100; i++){
		QueuePut(pQ, i);
	}
	printf("Termino la do_put\n");
	
	sem_post(sem_destroy);
}

void do_get(Queue_t *pQ, sem_t *sem_create, sem_t *sem_destroy){
	sem_wait(sem_create);

	printf("Arranco la do_get\n");
	for (int i = 0; i < 100; i++){
		QueueGet(pQ);
	}
	printf("Termino la do_get\n");

	sem_post(sem_destroy);
}

void do_monitoring(Queue_t *pQ, sem_t *sem_create, sem_t *sem_destroy){
	sem_wait(sem_create);

	printf("Arranco la do_monitoring\n");
	for (int i = 0; i < 10; i++){
		QueueMonitor(pQ);
	}
	printf("Termino la do_monitoring\n");

	sem_post(sem_destroy);
}


int main(int argc, const char *argv[]){
	
    // hago el mapeo al vector pQ
    Queue_t *pQ = mmap(NULL, sizeof(Queue_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	// Queue_t *pQ;

    //inicializo la la cola
    // QueueInit(pQ);
    sem_t *sem_create 	= mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *sem_destroy 	= mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	sem_init(sem_create, 1, 0);
	sem_init(sem_destroy, 1, 0);


    pid_t pid;

    pid = fork();

	if(pid > 0){					// parent
		pid = fork();
		if(pid > 0){				// parent
			pid = fork();
			if(pid > 0){			// parent
				do_monitoring(pQ, sem_create, sem_destroy);
				// Espero para que terminen todos los procesos
				// wait(NULL);
				// wait(NULL);
				// wait(NULL); // medio rancio
				for(int i=0; i < 3; ++i){
					sem_wait(sem_destroy);
				}

				printf("Terminaron todos los procesos\n");
				assert(QueueSize(pQ) == 0);
				
				QueueDestroy(pQ);
	    		munmap(pQ, sizeof(Queue_t));

			}else if(pid == 0){		// child 3
				do_get(pQ, sem_create, sem_destroy);
			}else{
				perror("error fork child 3");
				exit(EXIT_FAILURE);
			}
		}else if (pid == 0){		// child 2
			do_put(pQ, sem_create, sem_destroy);
		}else{
			perror("error fork child 2");
			exit(EXIT_FAILURE);
		}
	}else if(pid == 0){				// child 1
		do_init(pQ, sem_create);
		// do_init(&pQ);
	}else{
		perror("error fork child 1");
		exit(EXIT_FAILURE);
	}

	
    return 0;
}






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