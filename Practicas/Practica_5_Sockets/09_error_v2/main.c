#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "Queue.h"
#include "UDTs.h"

#define TOTAL_REQUEST 20
#define MAX_TRIES	5
#define NUM_THREADS 4

void* thread_producer(void* arg){
	
	Queue_t* pQ = (Queue_t*) arg;

	int req_id = 0;
	int error_id;	
	while(req_id != TOTAL_REQUEST){

		Req_t request;
		init_Req(&request, req_id);

		if( (error_id = QueuePut(pQ, &request)) < 0){
			fprintf(stderr, "thread_producer error_id %d\n", error_id);
			exit(1);
		}
		// printf("P - Cree request %d\n", req_id);
		// printf("P: Cree %d y en la cola dice %d\n", request.r_id, pQ->units[req_id % Q_SZ].r_id);
		
		req_id++;
	}

	pthread_exit(NULL);
}

typedef struct{
	Queue_t* in;
	Queue_t* out;
	pthread_mutex_t *out_mtx;
}queues_t;



void* thread_StageA(void* arg){
	queues_t* queues = (queues_t*) arg;

	int req_id = 0, error_id;
	Req_t request;

	while(req_id != TOTAL_REQUEST){

		if( (error_id = QueueGet(queues->in, &request)) < 0){
			fprintf(stderr, "thread_StageA-Put error_id %d\n", error_id);
			exit(1);
		}
		// addAction(&request, 'A', 0);
		usleep(request.times.ms_A * 100);

		if(pthread_mutex_lock(queues->out_mtx) != 0){
			printf("Error locking mutex\n");
			exit(1);
        }
		if( (error_id = QueuePut(queues->out, &request)) < 0){
			fprintf(stderr, "thread_StageA-Get error_id %d\n", error_id);
			exit(1);
		}
		printf("A: tengo %d, en salida dice %d\n", request.r_id, queues->out->units[(queues->out->idx_put-1) % Q_SZ].r_id);
		QueueMonitor(queues->in);
		QueueMonitor(queues->out);

		if(pthread_mutex_unlock(queues->out_mtx) != 0){
			printf("Error unlocking mutex\n");
			exit(1);
        }
		
		// printf("A - ");
		// printHistory(&request);
	
		req_id++;
	}

	pthread_exit(NULL);
}

void* thread_consumer(void* arg){
	
	Queue_t* pQ = (Queue_t*) arg;

	int req_id = 0, error_id;
	Req_t request;
	while(req_id != TOTAL_REQUEST){

		// printf("C - Antes del get");
		if( (error_id = QueueGet(pQ, &request)) < 0){
			fprintf(stderr, "Consumer-Get error_id %d\n", error_id);
			exit(1);
		}
		// printf("C - Despues del get");
		// addAction(&request, 'C', 2);
		usleep(request.times.ms_C * 100);

		printf("Consumer - ");
		printHistory(&request);
		fflush(stdout);
	
		req_id++;
	}

	pthread_exit(NULL);
}

typedef struct{
	Queue_t* in;
	Queue_t* out;
	pthread_mutex_t mtx_in;
	pthread_mutex_t mtx_out;
	pthread_cond_t new_input_flag;
	int idx_control;
}Sync_Control_t;

void init_SyncControl(Sync_Control_t* sync_var, Queue_t* in_, Queue_t* out_){
	sync_var->in 			= in_;
	sync_var->out 			= out_;
	sync_var->idx_control 	= 0;
	if(pthread_cond_init(&sync_var->new_input_flag, NULL)){
		printf("Fallo init cond");
		exit(1);
	}
	if(pthread_mutex_init(&sync_var->mtx_out, NULL)){
		printf("Fallo init cond");
		exit(1);
	}
	if(pthread_mutex_init(&sync_var->mtx_in, NULL)){
		printf("Fallo init cond");
		exit(1);
	}
}

void* thread_StageB(void* arg){
	Sync_Control_t* sync = (Sync_Control_t*) arg;

	int tries = 0, error_id;
	// Req_t request;
	pid_t x = syscall(__NR_gettid);

	while(tries != MAX_TRIES){
		Req_t request;

		// Tomo el acceso a la cola de entrada
		if(pthread_mutex_lock(&sync->mtx_in) != 0){
			printf("Error locking mutex\n");
			exit(1);
        }
		printf("Se supone que voy a agarrar R#%d\n", sync->in->units[sync->in->idx_get % Q_SZ].r_id);

		// Me fijo si esta vacia
		if(QueueSize(sync->in) == 0){
			if(pthread_mutex_unlock(&sync->mtx_in) != 0 ){
				printf("Error unlock\n");
				exit(1);
			}
			printf("vacia - espero\n");
			sleep(1);
			tries++;
			continue;
		}

		if( (error_id = QueueGet(sync->in, &request)) < 0){
			fprintf(stderr, "StageB-Get error_id %d\n", error_id);
			exit(1);
		}
		// printf("B - %d - Despues del get\n", x);
		if(pthread_mutex_unlock(&sync->mtx_in) != 0){
			printf("Error unlocking mutex\n");
			exit(1);
        }

		// Ya tengo la accion en este punto
		// addAction(&request, 'B', 1);
		printf("B - %d - Request #%d\n", x, request.r_id);
		// printHistory(&request);
		usleep(request.times.ms_B * 100);

		// Ahora tengo que ver si puedo ponerlo en la cola de salida
		if(pthread_mutex_lock(&sync->mtx_out) != 0){
			printf("Error locking mutex\n");
			exit(1);
        }

		while(sync->idx_control != request.r_id){	// Espero a que el indice coincida con el id del request
			pthread_cond_wait(&sync->new_input_flag, &sync->mtx_out);
			// printf("B - %d - Despues del wait -> %d - %d\n", x,sync->idx_control, request.id);
        }

		// ya puedo poner en la cola de la salida el request
		if( (error_id = QueuePut(sync->in, &request)) < 0){
			fprintf(stderr, "StageB-Put error_id %d\n", error_id);
			exit(1);
		}
		// printf("B - %d - Despues del Put\n", x);
		sync->idx_control++;

		// printf("B - %d - Aviso -> %d. Id es %d\n", x, sync->idx_control, request.id);
		if(pthread_cond_broadcast(&sync->new_input_flag) != 0){
    	    printf("Error cond signal\n");
			exit(1);
  		}

        // libero el mutex de acceso
        if(pthread_mutex_unlock(&sync->mtx_out) != 0 ){
			printf("Error cond signal\n");
			exit(1);
        }

		tries = 0;
	}

	printf("Salimos del thread\n");
	pthread_exit(NULL);
}

// void* threads_stageB

int main(){
	srand(time(0));

	Queue_t entry_A;
	Queue_t q_AB;
	Queue_t q_BC;
	QueueInit(&entry_A);
	QueueInit(&q_AB);
	QueueInit(&q_BC);

	pthread_t producer, stageA, stageB[NUM_THREADS], consumer;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	// pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	// ---------------------- productor
	if(pthread_create(&producer, &attr, thread_producer, (void*) &entry_A) < 0)
		exit(1);
	
	// ---------------------- stage A
	Sync_Control_t sync;
	init_SyncControl(&sync, &q_AB, &q_BC);
	// queues_t stageA_queues = {&entry_A, &q_AB};
	queues_t stageA_queues;
	stageA_queues.in = &entry_A;
	stageA_queues.out = &q_AB;
	stageA_queues.out_mtx = &sync.mtx_in;

	if(pthread_create(&stageA, &attr, thread_StageA, (void*) &stageA_queues) < 0)
		exit(1);
	
	// ---------------------- stage B
	// Ahora los stages de B
	
	// pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	for(int i=0; i < NUM_THREADS; ++i){
		if(pthread_create(&stageB[i], &attr, thread_StageB, (void*) &sync) < 0){
			exit(1);
		}
	}
	// ---------------------- Stage C
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	// if(pthread_create(&consumer, &attr, thread_consumer, (void*) &q_AB) < 0)
	// if(pthread_create(&consumer, &attr, thread_consumer, (void*) &entry_A) < 0)
	if(pthread_create(&consumer, &attr, thread_consumer, (void*) &q_BC) < 0)
		exit(1);


	pthread_join(producer, NULL);
	pthread_join(stageA, NULL);
	for(int i=0; i < NUM_THREADS; ++i)
		pthread_join(stageB[i], NULL);
	pthread_join(consumer, NULL);

	// sleep(200);
	QueueDestroy(&entry_A);
	QueueDestroy(&q_AB);
	QueueDestroy(&q_BC);

	pthread_exit(NULL);
	// return 0;
}