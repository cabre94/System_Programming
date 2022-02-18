#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <pthread.h>

#include "Queue.h"
// typedef struct WorkUnit_t;

// Inicializa (debe residir en un segmento de shared memory)
int QueueInit(Queue_t *pQ){
	pQ->idx_put = 0;
	pQ->idx_get = 0;
	pQ->size	= 0;

	int error_id;

	if( (error_id = pthread_mutex_init(&(pQ->mtx_sync), NULL)) != 0){
		fprintf(stderr, "Error %d in QueueInit->pthread_mutex_init\n", error_id);
		return error_id;
	}

	if((error_id = pthread_cond_init(&pQ->get_ready, NULL)) != 0){
		fprintf(stderr, "Error %d in QueueInit->pthread_cond_init\n", error_id);
		printf("Error en pthread_cond_init\n");
		pthread_mutex_destroy(&pQ->mtx_sync);
		return error_id;
	}

	if((error_id = pthread_cond_init(&pQ->put_ready, NULL)) != 0){
		fprintf(stderr, "Error %d in QueueInit->pthread_cond_init\n", error_id);
		pthread_mutex_destroy(&pQ->mtx_sync);
		pthread_cond_destroy(&pQ->put_ready);
		return error_id;
	}

	return 0;
}

// Desstruye el contenedor, liberando recursos
int QueueDestroy(Queue_t *pQ){
	int error_id;	// liberar conds si falla mutex?

	if( (error_id = pthread_mutex_destroy(&pQ->mtx_sync)) < 0 ){
		fprintf(stderr, "Error %d in QueueDestroy->pthread_mutex_destroy\n", error_id);
		return error_id;
	}
	if( (error_id = pthread_cond_destroy(&pQ->get_ready)) < 0 ){
		fprintf(stderr, "Error %d in QueueDestroy->pthread_cond_destroy\n", error_id);
		return error_id;
	}
	if( (error_id = pthread_cond_destroy(&pQ->put_ready)) < 0 ){
		fprintf(stderr, "Error %d in QueueDestroy->pthread_cond_destroy\n", error_id);
		return error_id;
	}
	
	return 0;
}

// Agrega un Nuevo elemento. Bloquea si no hay espacio
int QueuePut(Queue_t *pQ, Req_t* w_unit){
	int error_id;

	// tomo el mutex
	if( (error_id = pthread_mutex_lock(&(pQ->mtx_sync))) != 0 ){
		fprintf(stderr, "Error %d in QueuePut->pthread_mutex_lock\n", error_id);
		return error_id;
	}

	// espero a que la cola tenga lugar
	while(QueueSize(pQ) == Q_SZ){
		pthread_cond_wait(&pQ->put_ready, &pQ->mtx_sync);
	}

	pQ->units[pQ->idx_put % Q_SZ] = *w_unit;
	pQ->idx_put++;
	pQ->size++;

	// Mando señal de que se pueden sacar cosas
	if ((error_id = pthread_cond_signal(&(pQ->get_ready))) != 0){
		fprintf(stderr, "Error %d in QueuePut->pthread_cond_signal\n", error_id);
		return error_id;
	}

	// libero el mutex de acceso
	if( (error_id = pthread_mutex_unlock(&pQ->mtx_sync)) != 0 ){
		fprintf(stderr, "Error %d in QueuePut->pthread_mutex_unlock\n", error_id);
		return error_id;
	}

	return 0;
}

// Remueve y retorna un elemento, bloquea si no hay elementos
int QueueGet(Queue_t *pQ, Req_t* w_unit){
	int error_id;

	// tomo el mutex
	if( (error_id = pthread_mutex_lock(&(pQ->mtx_sync))) != 0 ){
		fprintf(stderr, "Error %d in QueueGet->pthread_mutex_lock\n", error_id);
		return error_id;
	}

	// Espero a que la cola tenga algo
	while(QueueSize(pQ) == 0){
		pthread_cond_wait(&pQ->get_ready, &(pQ->mtx_sync));
	}

	*w_unit = pQ->units[pQ->idx_get % Q_SZ];	// Guardo el coso
	pQ->idx_get++;
	pQ->size--;

	// Mando señal de que hay lugar por si alguien esta esperando
	if ((error_id = pthread_cond_signal(&(pQ->put_ready))) != 0){
		fprintf(stderr, "Error %d in QueueGet->pthread_cond_signal\n", error_id);
		return error_id;
	}

	// libero el mutex de acceso
	if( (error_id = pthread_mutex_unlock(&pQ->mtx_sync)) != 0 ){
		fprintf(stderr, "Error %d in QueueGet->pthread_mutex_unlock\n", error_id);
		return error_id;
	}

	return 0;
}

// recupera la cantidad de elementos en la cola
unsigned long QueueSize(Queue_t *pQ){
	return  pQ->size;

}

// // monitorea la cola
// void QueueMonitor(Queue_t *pQ){
// 	assert(sem_wait(&pQ->mtx_sync) != -1); // Por si alguien lo esta  usando

// 	// Hay elementos y nadie lo esta usando
// 	int sz = pQ->idx_put - pQ->idx_get;
// 	printf("-> ");
// 	for(int i = 0; i < sz; i++){
//         printf("%d -> ", pQ->elem[(pQ->idx_get + i) % Q_SZ]);
//     }
// 	putchar('\n');

// 	assert(sem_post(&pQ->mtx_sync) != -1); // Lo libero

// }


// int main(){

// 	return 0;
// }