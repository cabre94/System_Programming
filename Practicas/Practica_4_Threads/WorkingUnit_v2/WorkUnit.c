#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "WorkUnit.h"


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
	printf("In thread %ld: fake_func \"working\" during %ld sec.", id, sTime);
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

int update_statMonitor(StatMonitor_t *stat_monitor, WorkUnitStat_t *stat){
	int error_id;
	// Tomo el mutex
	if( (error_id = pthread_mutex_lock(&stat_monitor->access)) != 0){
		printf("stat mutex lock error: %d\n", error_id);
		return -1;
	}

	stat_monitor->n_WU++;		// una unidad mas en todo el proceso
	stat_monitor->t_waiting += (stat->startProcTime  - stat->submitTime);
	stat_monitor->t_proc += (stat->endProcTime - stat->startProcTime);

	// Largo el mutex
	if( (error_id = pthread_mutex_unlock(&stat_monitor->access)) != 0){
		printf("stat mutex unlock error: %d\n", error_id);
		return -1;
	}
}

int main(){

}