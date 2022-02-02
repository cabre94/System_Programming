#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#include "WorkUnit.h"


// ------------------------------- Queue_t
// Inicializa (debe residir en un segmento de shared memory)
int QueueInit(Queue_t *pQ){
	pQ->idx_put = 0;
	pQ->idx_get = 0;
	pQ->size 	= 0;

	int error_id;

	if( (error_id = pthread_mutex_init(&(pQ->mtx_sync), NULL)) != 0){
		printf("Error en pthread_mutex_init\n");
		return error_id;
	}

	if((error_id = pthread_cond_init(&pQ->get_ready, NULL)) != 0){
		printf("Error en pthread_cond_init\n");
		pthread_mutex_destroy(&pQ->mtx_sync);
		return error_id;
	}

	if((error_id = pthread_cond_init(&pQ->put_ready, NULL)) != 0){
		printf("Error en pthread_cond_init\n");
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
	if( (error_id = pthread_mutex_lock(&(pQ->mtx_sync))) != 0 ){
		printf("Error?\n");
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
	if( (error_id = pthread_mutex_lock(&(pQ->mtx_sync))) != 0 ){
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
		return error_id;
	}

	// libero el mutex de acceso
	if( (error_id = pthread_mutex_unlock(&pQ->mtx_sync)) != 0 ){
		return error_id;
	}

	return 0;
}

/* Toda un tarde revisando el codigo porque no me funcionaba por esta funcion del orto.
Aca no hay que tomar el mtex porque cuando llamamos a esta funcion en put y get
ya esta tomado. Termine poniendo un segundo atributo con el size */
unsigned long QueueSize(Queue_t *pQ){
	return  pQ->size;
}



// ------------------------------- WorkUnitStat_t
// Inicializa WorkUnitStat_t con todos ceros
void workUnitStat_init(WorkUnitStat_t* pWUS){
	pWUS->submitTime 	= 0;
	pWUS->startProcTime = 0;
	pWUS->endProcTime 	= 0;
}


// ------------------------------- WorkUnit_t
// Inicializa WorkUnit_t
void workUnit_init(WorkUnit_t *pWU, WorkUnitId id, void *context, ProcFunc_t fun){
	pWU->id = id;
	pWU->fun = fun;
	pWU->context = context;
	workUnitStat_init(&pWU->stats);
}

// ------------------------------- WorkerThread_t
int workerThread_init(WorkerThread_t *pWT, long id, Queue_t *pQueue, StatMonitor_t *pSMonitor){
	pWT->id = id;
	pWT->pQueue = pQueue;
	pWT->pMonitor = pSMonitor;

	printf("Initializing thread %ld\n", id);

	int error_id;
	if( (error_id = pthread_create(&(pWT->thr), NULL, thread_fun, (void *) pWT)) != 0){
		printf("Error creating thread: %d\n", error_id);
		return -1;
	}

	return 0;
}


// ------------------------------- WorkServer_t
typedef struct mycontext{
	WorkUnitId id;
	int th_id;
} ctx_t;

void* thread_fun(void* arg){

	WorkerThread_t *pWT = (WorkerThread_t *) arg;

	WorkUnit_t w_unit;

	while(1){
		// Tomo una workUnit de la cola, el mismo workerThread sabe si es propia o no
		QueueGet(pWT->pQueue, &w_unit);

		if(w_unit.fun == NULL)
			break;
		
		ctx_t ctx = {w_unit.id , pWT->id};
		
		w_unit.stats.startProcTime 	= time(NULL);	// Medio pedorro
		w_unit.fun((void *) &ctx);	// Que la unidad haga lo que tenga que hacer				
		w_unit.stats.endProcTime 	= time(NULL);	// Medio pedorro

		// Se la mando al monitor para que actualice las estadisticas
		statMonitor_update(pWT->pMonitor, &(w_unit.stats));
	}

	pthread_exit(NULL);
	// Como costo esto dios
}

// TODO me falta manejar bien los errores aca
int workServer_init(WorkServer_t *pWServer, int unique_Queue){
	printf("Inicializo Server con %d threads\n", NUM_WORKER_THREADS);

	pWServer->unique_Queue = unique_Queue;
	pWServer->idx_submit = 0;

	pWServer->pMonitor = (StatMonitor_t *) malloc(sizeof(StatMonitor_t));
	statMonitor_init(pWServer->pMonitor);

	if(unique_Queue){	// Una sola queque para todos los threads
		printf("Una unica queue\n");
		// Creo una unica queque
		pWServer->pQueue = (Queue_t *) malloc(sizeof(Queue_t));
		QueueInit(pWServer->pQueue);

		// Mando a crear todos los threads
		for(long i = 0; i < NUM_WORKER_THREADS; ++i){
			workerThread_init(&pWServer->workers[i], i, pWServer->pQueue, pWServer->pMonitor);
		}
	}else{				// Una queque por cada thread
		printf("Una queue por thread\n");
		pWServer->pQueue = (Queue_t *) malloc(NUM_WORKER_THREADS*sizeof(Queue_t));
		for(long i = 0; i < NUM_WORKER_THREADS; ++i){
			QueueInit(&pWServer->pQueue[i]);
			workerThread_init(&pWServer->workers[i], i, &pWServer->pQueue[i], pWServer->pMonitor);
		}
	}

	printf("Termino Server_init\n");
	return 0;

	// Tengo que allocar el servidor y la cola 
}

// Chequear si puedo usar var local
int workServer_destroy(WorkServer_t *pWServer){
	// Creo un workUnit auxiliar para liquidar el thread
	// NO se si hace falta allocar o podria ser var local
	WorkUnit_t end_WUnit;
	end_WUnit.fun = NULL;

	for (int i = 0; i < NUM_WORKER_THREADS; i++){
		// mando igual que la cantidad de threads, van a terminar
		// en vez de mandar sobre la cola del server lo mando sobre la
		// cola de cada worker (que puede ser la misma) para hacerlo mas generico
		QueuePut(pWServer->workers[i].pQueue, &end_WUnit);
	}

	// Hay que esperar que cada thread termine
	for (int i = 0; i < NUM_WORKER_THREADS; i++){
		pthread_join(pWServer->workers[i].thr, NULL);
		printf("Thread %ld joined\n", pWServer->workers[i].id);
	}

	// Imprimo las stats para comparar despues las dos modalidades
	statMonitor_print(pWServer->pMonitor);

	// Destruyo la o las queue
	if(pWServer->unique_Queue){
		QueueDestroy(pWServer->pQueue);
	}else{
		for(int i = 0; i < NUM_WORKER_THREADS; ++i){
			QueueDestroy(&pWServer->pQueue[i]);
		}
	}
	// Destruyo la StatMonitor
	statMonitor_destroy(pWServer->pMonitor);
	// libero la memoria
	free(pWServer->pQueue);
	free(pWServer->pMonitor);

	return 0;
}

int workServer_submit(WorkServer_t *pWServer, WorkUnit_t *pWUnit){
	pWUnit->stats.submitTime = time(NULL);
	// Hay que manejar las excepciones, pero quiero terminarlo
	if(pWServer->unique_Queue){
		QueuePut(pWServer->pQueue, pWUnit);
	}else{
		QueuePut(pWServer->workers[pWServer->idx_submit].pQueue, pWUnit);
		pWServer->idx_submit = (pWServer->idx_submit+1) % NUM_WORKER_THREADS;
	}

	return 0;
}

// ------------------------------- FakeWorkUnitGen_t
void fake_func(void* context){
	ctx_t *ctx = (ctx_t *) context;
	int sTime = rand() % 5;
	printf("Thread #%d with WorkUnit #%d \"working\" for %d sec.\n", ctx->th_id, ctx->id, sTime);
	sleep(sTime);
}

void fakeWorkUnitGen_init(FakeWorkUnitGen_t *pFWUGen, ProcFunc_t fake_fun){
	pFWUGen->fake_fun = fake_fun;
	pFWUGen->total_calls = 0;
}


void fakeWorkUnitGen_use(FakeWorkUnitGen_t *pFWUGen, WorkServer_t *pWServer, int num_calls){	
	
	WorkUnit_t WUnit;
	for (long i = 0; i < num_calls; i++){
		workUnit_init(&WUnit, pFWUGen->total_calls, (void*) pFWUGen->total_calls, pFWUGen->fake_fun);

		workServer_submit(pWServer, &WUnit);
		
		pFWUGen->total_calls++;
    }

	// No se si deberia allocar
}


// ------------------------------- StatMonitor_t
int statMonitor_init(StatMonitor_t *stat_monitor){
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

int statMonitor_update(StatMonitor_t *stat_monitor, WorkUnitStat_t *w_stat){
	int error_id;
	// Tomo el mutex
	if( (error_id = pthread_mutex_lock(&stat_monitor->access)) != 0){
		printf("stat mutex lock error: %d\n", error_id);
		return -1;
	}

	stat_monitor->n_WU++;		// una unidad mas en todo el proceso
	stat_monitor->t_waiting += (w_stat->startProcTime  - w_stat->submitTime);
	stat_monitor->t_proc 	+= (w_stat->endProcTime - w_stat->startProcTime);

	// Largo el mutex
	if( (error_id = pthread_mutex_unlock(&stat_monitor->access)) != 0){
		printf("stat mutex unlock error: %d\n", error_id);
		return -1;
	}

	return 0;
}

int statMonitor_print(StatMonitor_t *stat_monitor){
	int error_id;
	// Tomo el mutex
	if( (error_id = pthread_mutex_lock(&stat_monitor->access)) != 0){
		printf("stat mutex lock error: %d\n", error_id);
		return -1;
	}

	printf("Unidades totales: %d\n", 			stat_monitor->n_WU);
	printf("Tiempo promedio en espera: %f\n", 	(double) stat_monitor->t_waiting / stat_monitor->n_WU);
	printf("Tiempo promedio procesando: %f\n", 	(double) stat_monitor->t_proc / stat_monitor->n_WU);

	// Largo el mutex
	if( (error_id = pthread_mutex_unlock(&stat_monitor->access)) != 0){
		printf("stat mutex unlock error: %d\n", error_id);
		return -1;
	}

	return 0;
}

int statMonitor_destroy(StatMonitor_t *stat_monitor){
	int error_id;
	if( (error_id = pthread_mutex_destroy(&stat_monitor->access)) != 0 ){
		printf("stat mutex destroy error: %d\n", error_id);
		return -1;
	}
	return 0;
}


// ------------------------------- main
int main(){
	srand(time(NULL));

	WorkServer_t workServer;
	FakeWorkUnitGen_t fakeGenerator;

	workServer_init(&workServer, 1);
	// workServer_init(&workServer, 0);

	fakeWorkUnitGen_init(&fakeGenerator, fake_func);

	fakeWorkUnitGen_use(&fakeGenerator, &workServer, 50);
	// printf("Main(): sleep 10 sec\n");
	// sleep(10);
	
	// printf("Main(): Mando otros 10 trabajos\n");
	// fakeWorkUnitGen_use(&fakeGenerator, &workServer, 50);

	workServer_destroy(&workServer);

	return 0;

}