#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <pthread.h>
#include <assert.h>

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
	while(pQ->size == Q_SZ){
	// while(QueueSize(pQ) == Q_SZ){
		pthread_cond_wait(&pQ->put_ready, &pQ->mtx_sync);
	}

	pQ->units[pQ->idx_put % Q_SZ] = *w_unit;
	int kk = pQ->units[pQ->idx_put % Q_SZ].r_id;
	// printf("En put, tengo %d y se guardo %d\n", w_unit->r_id, kk);
	// fflush(stdout);
	assert(w_unit->r_id == kk);
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
	// if( (error_id = pthread_mutex_lock(&(pQ->mtx_sync))) != 0 ){
	// 	printf("Error %d in QueueGet->pthread_mutex_lock\n", error_id);
	// 	return error_id;
	// }

	// 
	struct timespec ts;
	ts.tv_nsec = 0;
	ts.tv_sec  = QUEUE_TIMEOUT;
	if((error_id = pthread_mutex_timedlock(&pQ->mtx_sync, &ts)) != 0 ){
		if(error_id == ETIMEDOUT){
			return error_id;
		}else{
			printf("Error %d in QueueGet->pthread_mutex_lock\n", error_id);
			return error_id;
		}
	}





	// Espero a que la cola tenga algo
	while((pQ->size) == 0){
	// while(QueueSize(pQ) == 0){
		pthread_cond_wait(&pQ->get_ready, &(pQ->mtx_sync));
	}

	*w_unit = pQ->units[pQ->idx_get % Q_SZ];	// Guardo el coso
	// printf("En get, tengo %d y se guardo %d\n", pQ->units[pQ->idx_get % Q_SZ].r_id, w_unit->r_id);
	assert(pQ->units[pQ->idx_get % Q_SZ].r_id == w_unit->r_id);
	pQ->idx_get++;
	pQ->size--;

	// Mando señal de que hay lugar por si alguien esta esperando
	if ((error_id = pthread_cond_signal(&(pQ->put_ready))) != 0){
		printf("Error %d in QueueGet->pthread_cond_signal\n", error_id);
		return error_id;
	}

	// libero el mutex de acceso
	if( (error_id = pthread_mutex_unlock(&pQ->mtx_sync)) != 0 ){
		printf("Error %d in QueueGet->pthread_mutex_unlock\n", error_id);
		return error_id;
	}

	return 0;
}

// recupera la cantidad de elementos en la cola
unsigned long QueueSize(Queue_t *pQ){
	return  pQ->size;

}

// monitorea la cola
int QueueMonitor(Queue_t *pQ, char prefix){
	int error_id;
	if( (error_id = pthread_mutex_lock(&(pQ->mtx_sync))) != 0 ){
		printf("Error %d in QueueGet->pthread_mutex_lock\n", error_id);
		return error_id;
	}

	// Hay elementos y nadie lo esta usando
	int sz = pQ->idx_put - pQ->idx_get;
	printf("%c: -> ", prefix);
	for(int i = 0; i < sz; i++){
        printf("%d -> ", pQ->units[(pQ->idx_get + i) % Q_SZ].r_id);
    }
	putchar('\n');
	fflush(stdout);

	if( (error_id = pthread_mutex_unlock(&pQ->mtx_sync)) != 0 ){
		printf("Error %d in QueueGet->pthread_mutex_unlock\n", error_id);
		return error_id;
	}

	return 0;
}


// int main(){

// 	return 0;
// }