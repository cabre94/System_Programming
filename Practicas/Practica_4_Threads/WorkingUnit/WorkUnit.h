#ifndef WORKUNIT_H
#define WORKUNIT_H

// #pragma once

#include <pthread.h>
#include <time.h>


typedef void (*ProcFunc_t)(void *ctx);
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


// Inicializa WorkUnitStat_t con todos ceros
void init_WorkUnitStat(WorkUnitStat_t* pWUS);
// Inicializa WorkUnit_t
void WorkUnitInit(WorkUnit_t *pWU, WorkUnitId id, void *context, ProcFunc_t fun);



#endif // !WORKUNIT_H



