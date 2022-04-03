#ifndef _UDTS_H
#define _UDTS_H

#include <iostream>
#include <cstdio>

#define TIME_RANGE 1000

using Tick = unsigned int;
using Request_Id = unsigned int;

struct Data_t{
	Tick ms_A, ms_B, ms_C;
};

class Req_t{
public:
	Req_t() : id(0), actions(""){
		times.ms_A = (rand() % TIME_RANGE) + 1000;
		times.ms_B = (rand() % TIME_RANGE) + 1000;
		times.ms_C = (rand() % TIME_RANGE) + 1000;
	}
	void setID(Request_Id id_){
		id = id_;
	}
	void addAction(char c){
		actions.push_back(c);
	}
	void printActions(){
		// std::cout << "Request #" << id << ": --> " << actions << std::endl;
	}
	// void printHistory(){
	// 	std::cout << "Request #" << id << ": --> " << actions << ".";
	// 	std::cout << " A: " << times.ms_A / 1000.0 << "s";
	// 	std::cout << " B: " << times.ms_B / 1000.0 << "s";
	// 	std::cout << " C: " << times.ms_C / 1000.0 << "s.";
	// 	std::cout << " Total: " << (times.ms_A+times.ms_B+times.ms_C) / 1000.0 << "s." << std::endl;
	// }
	void printHistory(){
		printf("Request #%d\n", id);
	}
private:
	Request_Id id;
	std::string actions;
	Data_t times;
};


#endif // !_UDTS_H