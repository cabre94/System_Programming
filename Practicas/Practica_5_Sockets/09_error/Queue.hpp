#ifndef _QUEUE_H
#define _QUEUE_H

#include <pthread.h>
#include <string>
#include <iostream>

#include "UDTs.hpp"

#define QUEQUE_SZ 10

template<class T>
class Queue{
public:
	Queue(){
		idx_put = 0;
        idx_get = 0;
        sz = 0;

        if(pthread_mutex_init(&mtx_sync, NULL) != 0){
			throw std::string("pthread_mutex_init");
        }

        if(pthread_cond_init(&get_ready, NULL) != 0){
			pthread_mutex_destroy(&mtx_sync);
			throw std::string("pthread_cond_init");
        }

        if(pthread_cond_init(&put_ready, NULL) != 0){
			pthread_mutex_destroy(&mtx_sync);
			pthread_cond_destroy(&put_ready);
			throw std::string("pthread_cond_init");
        }
	}
	~Queue(){
		pthread_mutex_destroy(&mtx_sync);                                                                                                                                               
        pthread_cond_destroy(&get_ready);                                                                                                                                               
        pthread_cond_destroy(&put_ready);                                                                                                                                               
        // if(pthread_mutex_destroy(&mtx_sync) < 0 ){                                                                                                                                               
		// 	throw std::string("pthread_mutex_destroy");
        // }                                                                                                                                                                                                          
        // if(pthread_cond_destroy(&get_ready) < 0 ){                                                                                                                                               
		// 	throw std::string("pthread_cond_destroy");
        // }                                                                                                                                                                                                          
        // if(pthread_cond_destroy(&put_ready) < 0 ){                                                                                                                                               
		// 	throw std::string("pthread_cond_destroy");
        // }
	}
	void put(T unit){
		// tomo el mutex
        if(pthread_mutex_lock(&mtx_sync) != 0)
			throw std::string("pthread_mutex_lock");

        // espero a que la cola tenga lugar
        while(size() == QUEQUE_SZ)
			pthread_cond_wait(&put_ready, &mtx_sync);

        units[idx_put % QUEQUE_SZ] = unit;
        idx_put++;
		sz++;

        // Mando señal de que se pueden sacar cosas
        if(pthread_cond_signal(&get_ready) != 0)
			throw std::string("pthread_cond_signal");

        // libero el mutex de acceso
        if(pthread_mutex_unlock(&mtx_sync) != 0)
			throw std::string("pthread_cond_signal");

	}
	T get(){
		// tomo el mutex
		int error_id;
        if( (error_id = pthread_mutex_lock(&mtx_sync)) != 0){
			std::cout << "1" << std::endl;
			std::cout << error_id << std::endl;
			throw std::string("pthread_mutex_lock");
        }

        // Espero a que la cola tenga algo
        while(size() == 0){
			pthread_cond_wait(&get_ready, &mtx_sync);
        }

        T unit = units[idx_get % QUEQUE_SZ];        // Guardo el coso
    	idx_get++;
    	sz--;

        // Mando señal de que hay lugar por si alguien esta esperando
        if(pthread_cond_signal(&put_ready) != 0){
			std::cout << "2" << std::endl;
			throw std::string("pthread_cond_signal");
    	}

        // libero el mutex de acceso
        if(pthread_mutex_unlock(&mtx_sync) != 0){
			std::cout << "3" << std::endl;
			throw std::string("pthread_mutex_unlock");
        }

		return unit;
		// return Req_t();
	}
	size_t size(){
		return sz;
	}
private:
	pthread_mutex_t mtx_sync;
	pthread_cond_t get_ready;
	pthread_cond_t put_ready;
	T units[QUEQUE_SZ];
	size_t idx_put;
	size_t idx_get;
	size_t sz;
};





#endif // !_QUEUE_H