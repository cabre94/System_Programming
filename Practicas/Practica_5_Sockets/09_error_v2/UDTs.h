#ifndef _UDTS_H
#define _UDTS_H

#include <stdio.h>
#include <string.h>


#define TIME_RANGE 1000

typedef struct {
	int ms_A, ms_B, ms_C;
}Data_t;

typedef struct{ 
	int r_id;
	Data_t times;
	// char actions[10];
}Req_t;

void init_Req(Req_t* pReq, int r_id_);
void addAction(Req_t* req, char c, int idx);
void printActions(Req_t* req);
void printHistory(Req_t* req);

// void printHistory(){
	// 	std::cout << "Request #" <<r_id << ": --> " << actions << ".";
	// 	std::cout << " A: " << times.ms_A / 1000.0 << "s";
	// 	std::cout << " B: " << times.ms_B / 1000.0 << "s";
	// 	std::cout << " C: " << times.ms_C / 1000.0 << "s.";
	// 	std::cout << " Total: " << (times.ms_A+times.ms_B+times.ms_C) / 1000.0 << "s." << std::endl;
	// }


#endif // !_UDTS_H