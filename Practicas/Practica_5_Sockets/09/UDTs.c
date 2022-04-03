#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UDTs.h"
#include "Queue.h"


void init_Req(Req_t* pReq, int id_){
	pReq->r_id = id_;

	pReq->times.ms_A = (rand() % TIME_RANGE) + 1000;
	pReq->times.ms_B = (rand() % TIME_RANGE) + 1000;
	pReq->times.ms_C = (rand() % TIME_RANGE) + 1000;
}

void print_id(Req_t* pReq){
	printf("Request #%d\n", pReq->r_id);
}


void printActions(Req_t* req){
	printf("Request #%d\n", req->r_id);
}

void print_idWTimes(Req_t* pReq){
	printf("Request #%d: -->", pReq->r_id);
	printf(" A: %lfs", pReq->times.ms_A / 1000.0);
	printf(" B: %lfs", pReq->times.ms_B / 1000.0);
	printf(" C: %lfs", pReq->times.ms_C / 1000.0);
	printf(" Total: %lfs\n",
		  (pReq->times.ms_A+pReq->times.ms_B+pReq->times.ms_C) / 1000.0);
}

