#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <WorkUnit.h>






void init_WorkUnitStat(WorkUnitStat_t* pWUS){
	pWUS->submitTime 	= 0;
	pWUS->startProcTime = 0;
	pWUS->endProcTime 	= 0;
}


void WorkUnitInit(WorkUnit_t *pWU, WorkUnitId id, void *context, ProcFunc_t fun){
	pWU->id = id;
	pWU->fun = fun;
	pWU->context = context;
	init_WorkUnitStat(&pWU->stats);
}


