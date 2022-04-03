#ifndef _UDTS_H
#define _UDTS_H

#include <stdio.h>
#include <string.h>

// #include "Queue.h"

struct Queue_t;

#define TIME_RANGE 1000

typedef struct {
	int ms_A, ms_B, ms_C;
}Data_t;

typedef struct{ 
	int r_id;
	Data_t times;
}Req_t;

void init_Req(Req_t* pReq, int r_id_);
void print_id(Req_t* pReq);
void print_idWTimes(Req_t* pReq);




#endif // !_UDTS_H