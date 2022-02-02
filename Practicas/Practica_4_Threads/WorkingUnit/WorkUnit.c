#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "WorkUnit.h"



// ------------------------------- Queue_t
// Inicializa (debe residir en un segmento de shared memory)
int QueueInit(Queue_t *pQ){
	pQ->idx_put = 0;
	pQ->idx_get = 0;

	int error_id;

	if((error_id = pthread_mutex_init(&pQ->mtx_sync, NULL)) != 0){
		return error_id;
	}

	if((error_id = pthread_cond_init(&pQ->get_ready, NULL)) != 0){
		pthread_mutex_destroy(&pQ->mtx_sync);
		return error_id;
	}

	if((error_id = pthread_cond_init(&pQ->put_ready, NULL)) != 0){
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
		return error_id;
	}
	if( (error_id = pthread_cond_destroy(&pQ->get_ready)) < 0 ){
		return error_id;
	}
	if( (error_id = pthread_cond_destroy(&pQ->put_ready)) < 0 ){
		return error_id;
	}
	
	return 0;
}

// Agrega un Nuevo elemento. Bloquea si no hay espacio
int QueuePut(Queue_t *pQ, WorkUnit_t* w_unit){
	int error_id;

	// tomo el mutex
	if( (error_id = pthread_mutex_lock(&pQ->mtx_sync)) != 0 ){
		return error_id;
	}

	// espero a que la cola tenga lugar
	while(QueueSize(pQ) != Q_SZ){
		pthread_cond_wait(&pQ->put_ready, &pQ->mtx_sync);
	}

	pQ->units[pQ->idx_put % Q_SZ] = *w_unit;
	pQ->idx_put++;

	// Mando señal de que se pueden sacar cosas
	if ((error_id = pthread_cond_signal(&(pQ->get_ready))) != 0){
        return error_id;
    }

	// libero el mutex de acceso
	if( (error_id = pthread_mutex_unlock(&pQ->mtx_sync)) != 0 ){
		return error_id;
	}

	return 0;
}

// Remueve y retorna un elemento, bloquea si no hay elementos
int QueueGet(Queue_t *pQ, WorkUnit_t* w_unit){
	int error_id;

	// tomo el mutex
	if( (error_id = pthread_mutex_lock(&pQ->mtx_sync)) != 0 ){
		return error_id;
	}

	// Espero a que la cola tenga algo
	while(QueueSize(pQ) != 0){
		pthread_cond_wait(&pQ->get_ready, &pQ->mtx_sync);
	}

	*w_unit = pQ->units[pQ->idx_get % Q_SZ];	// Guardo el coso
    pQ->idx_get++;

	// Mando señal de que hay lugar por si alguien esta esperando
	if ((error_id = pthread_cond_signal(&(pQ->put_ready))) != 0){
        return error_id;
    }

	// libero el mutex de acceso
	if( (error_id = pthread_mutex_unlock(&pQ->mtx_sync)) != 0 ){
		return error_id;
	}

	return 0;
}

// recupera la cantidad de elementos en la cola
unsigned long QueueSize(Queue_t *pQ){
	int error_id;

	// tomo el mutex
	if( (error_id = pthread_mutex_lock(&pQ->mtx_sync)) != 0 ){
		return error_id;
	}

	// Hay elementos y nadie lo esta usando
	unsigned long sz = pQ->idx_put - pQ->idx_get;

	// libero el mutex de acceso
	if( (error_id = pthread_mutex_unlock(&pQ->mtx_sync)) != 0 ){
		return error_id;
	}

	return sz;
}




// ------------------------------- WorkUnitStat_t
// Inicializa WorkUnitStat_t con todos ceros
void init_WorkUnitStat(WorkUnitStat_t* pWUS){
	pWUS->submitTime 	= 0;
	pWUS->startProcTime = 0;
	pWUS->endProcTime 	= 0;
}


// ------------------------------- WorkUnit_t
// Inicializa WorkUnit_t
void WorkUnitInit(WorkUnit_t *pWU, WorkUnitId id, void *context, ProcFunc_t fun){
	pWU->id = id;
	pWU->fun = fun;
	pWU->context = context;
	init_WorkUnitStat(&pWU->stats);
}
// ------------------------------- WorkerThread_t

// ------------------------------- WorkServer_t

// ------------------------------- FakeWorkUnitGen_t
void fake_func(void* context){
	long id = (long) context;
	int sTime = rand() % 5;
	printf("In thread %ld: fake_func \"working\" during %d sec.", id, sTime);
	sleep(sTime);
}

void init_FakeWorkUnitGen(FakeWorkUnitGen_t *fake_gen, ProcFunc_t fake_fun){
	fake_gen->fake_fun = fake_fun;
}

void useFakeGenerator(WorkServer_t *server){
	//
}


// ------------------------------- StatMonitor_t
int init_StatMonitor(StatMonitor_t *stat_monitor){
	stat_monitor->n_WU 		= 0;
	stat_monitor->t_waiting = 0;
	stat_monitor->t_proc 	= 0;

	int error_id;
	if( (error_id = pthread_mutex_init(&stat_monitor->access, NULL)) != 0){
		printf("stat mutex init error: %d\n", error_id);
		return -1;
	}
	
	return 0;
}

int update_statMonitor(StatMonitor_t *stat_monitor, WorkUnitStat_t *w_stat){
	int error_id;
	// Tomo el mutex
	if( (error_id = pthread_mutex_lock(&stat_monitor->access)) != 0){
		printf("stat mutex lock error: %d\n", error_id);
		return -1;
	}

	stat_monitor->n_WU++;		// una unidad mas en todo el proceso
	stat_monitor->t_waiting += (w_stat->startProcTime  - w_stat->submitTime);
	stat_monitor->t_proc += (w_stat->endProcTime - w_stat->startProcTime);

	// Largo el mutex
	if( (error_id = pthread_mutex_unlock(&stat_monitor->access)) != 0){
		printf("stat mutex unlock error: %d\n", error_id);
		return -1;
	}

	return 0;
}

int main(){

}