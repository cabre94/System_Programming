#include <iostream>
#include <unistd.h>

#include "Queue.hpp"
#include "UDTs.hpp"

#define TOTAL_REQUEST 5



// class Stage {
// public:
// 	// Stage() : state(IDLE) {}
// 	virtual void doWork(Tick t) = 0;    // todo Stage instanciable tiene que implementar este servicio

// // protected:
// // 	enum State { IDLE, WORKING};
// // 	State state;
// };


void* thread_producer(void* arg){
	
	Queue<Req_t>* entry = (Queue<Req_t>*) arg;

	Request_Id req_id = 0;
	while(req_id != TOTAL_REQUEST){

		// printf("thread_producer");
		Req_t request;
		request.setID(req_id);

		entry->put(request);
		
		req_id++;
	}

	pthread_exit(NULL);
}

void* thread_consumer(void* arg){
	Queue<Req_t>* entry = (Queue<Req_t>*) arg;

	Request_Id req_id = 0;
	while(req_id != TOTAL_REQUEST){

		printf("Hola %ld\n", entry->size());
		// request.printHistory();
		Req_t request = entry->get();
	
		req_id++;
		sleep(1);
	}

	pthread_exit(NULL);
}


int main(){
	srand(time(0));

	Queue<Req_t> entry_A;
	Queue<Req_t> q_AB;
	Queue<Req_t> q_CB;

	pthread_t producer, consumer;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	try{
		if(pthread_create(&producer, &attr, thread_producer, (void*) &entry_A) < 0)
			exit(1);
		
		pthread_join(producer, NULL);
		
		if(pthread_create(&consumer, &attr, thread_consumer, (void*) &entry_A) < 0)
			exit(1);
	}catch(const std::exception& e){
		std::cerr << e.what() << '\n';
	}
	catch(const std::string& e){
		std::cout << "Hola, no se que agarre" << std::endl;
	}
	// catch(...){
	// 	std::cout << "Hola, no se que agarre" << std::endl;
	// }

	std::cout << "Sali?" << std::endl;
	

	



	// Req_t kk;
	// for(int i=0; i < TOTAL_REQUEST; ++i){
	// 	kk = entry_A.get();
	// 	kk.printHistory();
	// }
	// kk = entry_A.get();


	pthread_exit(NULL);
	// return 0;
}