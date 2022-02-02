#ifndef WORKUNIT_H
#define WORKUNIT_H

// #pragma once

#include <pthread.h>
#include <time.h>

#include "Queue.h"

#define NUM_WORKER_THREADS 10


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
	pthread_t thr;
	long id;

	Queue_t *pQWorker;	// Para el caso b, donde cada thread tiene su cola
} WorkerThread_t;

typedef struct{
	WorkerThread_t workers[NUM_WORKER_THREADS];

	Queue_t *pQServer;	// Para el caso a, donde hay una unica cola
} WorkServer_t;


typedef struct{
	// ¿?
	ProcFunc_t fake_fun;
} FakeWorkUnitGen_t;

typedef struct{
	// ¿?
	int n_WU;
	double t_waiting;
	double t_proc;

	pthread_mutex_t access;
} StatMonitor_t;

// ------------------------------- WorkUnitStat_t
// Inicializa WorkUnitStat_t con todos ceros
void init_WorkUnitStat(WorkUnitStat_t* pWUS);


// ------------------------------- WorkUnit_t
// Inicializa WorkUnit_t
void WorkUnitInit(WorkUnit_t *pWU, WorkUnitId id, void *context, ProcFunc_t fun);

// ------------------------------- WorkerThread_t

// ------------------------------- WorkServer_t

// ------------------------------- FakeWorkUnitGen_t
void fake_func(void* context);

void init_FakeWorkUnitGen(FakeWorkUnitGen_t *fake_gen, ProcFunc_t fake_fun);

void useFakeGenerator(WorkServer_t *server);

// ------------------------------- StatMonitor_t
int init_StatMonitor(StatMonitor_t *stat_monitor);

int update_statMonitor(StatMonitor_t *stat_monitor, WorkUnitStat_t *stat);







#endif // !WORKUNIT_H



