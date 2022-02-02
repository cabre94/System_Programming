#ifndef WORKUNIT_H
#define WORKUNIT_H

// #pragma once

#include <pthread.h>
#include <time.h>

#define NUM_WORKER_THREADS 5
#define Q_SZ 10


typedef void (*ProcFunc_t)(void *context);
typedef unsigned int WorkUnitId;

typedef struct {
	time_t submitTime;		// submission time
	time_t startProcTime;	// processing start time
	time_t endProcTime;		// processing end time
} WorkUnitStat_t;

typedef struct{
    WorkUnitId 		id;
    ProcFunc_t 		fun;
    void 			*context;
    WorkUnitStat_t 	stats;
} WorkUnit_t;

typedef struct{
    pthread_mutex_t mtx_sync;		// sinc de acceso, mutex
    pthread_cond_t  get_ready;		// cant. de entradas libres
    pthread_cond_t 	put_ready;		// condicion para hacer put en la queue
    WorkUnit_t units[Q_SZ];
    unsigned int idx_put;
    unsigned int idx_get;
	unsigned long size;
} Queue_t;

typedef struct{
	// ¿?
	long total_calls;
	ProcFunc_t fake_fun;
} FakeWorkUnitGen_t;

typedef struct{
	// ¿?
	int n_WU;
	double t_waiting;
	double t_proc;

	pthread_mutex_t access;
} StatMonitor_t;


typedef struct{
	pthread_t thr;
	long id;			// Deberia ser el id del thread no del unit

	Queue_t *pQueue;	// Para el caso b, donde cada thread tiene su cola
	StatMonitor_t *pMonitor;
} WorkerThread_t;

typedef struct{
	int unique_Queue;
	int idx_submit;
	WorkerThread_t workers[NUM_WORKER_THREADS];

	Queue_t *pQueue;	// Para el caso a, donde hay una unica cola
	StatMonitor_t *pMonitor;
} WorkServer_t;

// ------------------------------- Queue_t
// Inicializa (debe residir en un segmento de shared memory)
int QueueInit(Queue_t *pQ);
// Desstruye el contenedor, liberando recursos
int QueueDestroy(Queue_t *pQ);
// Agrega un Nuevo elemento. Bloquea si no hay espacio
int QueuePut(Queue_t *pQ, WorkUnit_t* w_unit);
// Remueve y guarda un elemento, bloquea si no hay elementos
int QueueGet(Queue_t *pQ, WorkUnit_t* w_unit);
// recupera la cantidad de elementos en la cola
unsigned long QueueSize(Queue_t *pQ);
// monitorea la cola
// void QueueMonitor(Queue_t *pQ);



// ------------------------------- WorkUnitStat_t
// Inicializa WorkUnitStat_t con todos ceros
void WorkUnitStat_init(WorkUnitStat_t* pWUS);


// ------------------------------- WorkUnit_t
// Inicializa WorkUnit_t
void workUnit_init(WorkUnit_t *pWU, WorkUnitId id, void *context, ProcFunc_t fun);

// ------------------------------- WorkerThread_t
int workerThread_init(WorkerThread_t *pWT, long id, Queue_t *pQueue, StatMonitor_t *pSMonitor);

// ------------------------------- WorkServer_t
void* thread_fun(void* arg);

int workServer_init(WorkServer_t *pWServer, int unique_Queue);
// int WorkServer_init(WorkServer_t *pWServer);

int workServer_destroy(WorkServer_t *pWServer);

int workServer_submit(WorkServer_t *pWServer, WorkUnit_t *pWUnit);

// ------------------------------- FakeWorkUnitGen_t
void fake_func(void* context);

void fakeWorkUnitGen_init(FakeWorkUnitGen_t *fake_gen, ProcFunc_t fake_fun);

void fakeWorkUnitGen_use(FakeWorkUnitGen_t *pFWUGen, WorkServer_t *pWServer, int num_calls);

// ------------------------------- StatMonitor_t
int statMonitor_init(StatMonitor_t *stat_monitor);

int statMonitor_update(StatMonitor_t *stat_monitor, WorkUnitStat_t *stat);

int statMonitor_print(StatMonitor_t *stat_monitor);

int statMonitor_destroy(StatMonitor_t *stat_monitor);


#endif // !WORKUNIT_H
