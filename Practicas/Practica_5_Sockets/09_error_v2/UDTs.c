#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UDTs.h"


void init_Req(Req_t* req, int id_){
	req->id = id_;

	// memset(req->actions, 0, sizeof(req->actions));

	req->times.ms_A = (rand() % TIME_RANGE) + 1000;
	req->times.ms_B = (rand() % TIME_RANGE) + 1000;
	req->times.ms_C = (rand() % TIME_RANGE) + 1000;
}

void addAction(Req_t* req, char c, int idx){
	// req->actions[idx] = c;
}

void printActions(Req_t* req){
	printf("Request #%d\n", req->id);
	// fflush(stdout);
}

void printHistory(Req_t* req){
	printActions(req); return;
	// printf("Request #%d: --> %s.", req->id, req->actions);
	printf(" A: %lfs", req->times.ms_A / 1000.0);
	printf(" B: %lfs", req->times.ms_B / 1000.0);
	printf(" C: %lfs", req->times.ms_C / 1000.0);
	printf(" Total: %lfs\n", (req->times.ms_A+req->times.ms_B+req->times.ms_C) / 1000.0);
}

