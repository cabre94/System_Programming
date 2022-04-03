#include <unistd.h>
#include <pthread.h>

#include "Queue.h"
#include "UDTs.h"

#define NUM 30
#define MAX_TRIES 5
#define NUM_THREADS 5

typedef struct{
	Queue_t* pIn;
	pthread_mutex_t* mtx_in;
	pthread_cond_t* in_ready;
	int* in_sz;

	Queue_t* pOut;
	pthread_mutex_t* mtx_out;
	pthread_cond_t* out_ready;
	int* out_sz;
}Q_sync_t;

void init_QSync(Q_sync_t* sync, Queue_t* in, Queue_t* out, pthread_mutex_t* mtx_in,
	pthread_mutex_t* mtx_out, pthread_cond_t* cond_in, pthread_cond_t* cond_out, int* in_sz, int* out_sz){
	
	sync->pIn = in;
	sync->mtx_in = mtx_in;
	sync->in_ready = cond_in;
	sync->in_sz = in_sz;

	sync->pOut = out;
	sync->mtx_out = mtx_out;
	sync->out_ready = cond_out;
	sync->out_sz = out_sz;
}


void* thread_producer(void* arg){
	Q_sync_t* pQ_sync = (Q_sync_t*) arg;

	Req_t request;
	for(int i=0; i < NUM; ++i){
		init_Req(&request, i);

		pthread_mutex_lock(pQ_sync->mtx_out);

		while((*pQ_sync->out_sz) == Q_SZ){
			pthread_cond_wait(pQ_sync->out_ready, pQ_sync->mtx_out);
		}

		QueuePut(pQ_sync->pOut, &request);
		(*pQ_sync->out_sz)++;
		QueueMonitor(pQ_sync->pOut, 'P');
		pthread_cond_signal(pQ_sync->out_ready);
		
		pthread_mutex_unlock(pQ_sync->mtx_out);
	}

	pthread_exit(NULL);
}

void* thread_A(void* arg){
	Q_sync_t* pQ_sync = (Q_sync_t*) arg;

	Req_t request;
	while(1){

		// IN
		pthread_mutex_lock(pQ_sync->mtx_in);

		while((*pQ_sync->in_sz) == 0){
			pthread_cond_wait(pQ_sync->in_ready, pQ_sync->mtx_in);
		}
		QueueGet(pQ_sync->pIn, &request);
		(*pQ_sync->in_sz)--;
		// QueueMonitor(pQ_sync->pIn, 'A');
		pthread_cond_signal(pQ_sync->in_ready);
		
		pthread_mutex_unlock(pQ_sync->mtx_in);

		// OUT
		pthread_mutex_lock(pQ_sync->mtx_out);

		while((*pQ_sync->out_sz) == Q_SZ){
			pthread_cond_wait(pQ_sync->out_ready, pQ_sync->mtx_out);
		}
		QueuePut(pQ_sync->pOut, &request);
		(*pQ_sync->out_sz)++;
		// QueueMonitor(pQ_sync->pOut);
		pthread_cond_signal(pQ_sync->out_ready);
		// pthread_cond_broadcast(pQ_sync->out_ready);
		
		pthread_mutex_unlock(pQ_sync->mtx_out);
	}
	pthread_exit(NULL);
}

// void 

void* thread_C(void* arg){
	Q_sync_t* pQ_sync = (Q_sync_t*) arg;

	Req_t request;
	while(1){

		// IN
		pthread_mutex_lock(pQ_sync->mtx_in);

		while((*pQ_sync->in_sz) == 0){
			pthread_cond_wait(pQ_sync->in_ready, pQ_sync->mtx_in);
		}
		QueueGet(pQ_sync->pIn, &request);
		(*pQ_sync->in_sz)--;
		QueueMonitor(pQ_sync->pIn, 'C');
		pthread_cond_signal(pQ_sync->in_ready);
		
		pthread_mutex_unlock(pQ_sync->mtx_in);
	}
	pthread_exit(NULL);
}

int main(){

	Queue_t q1, q2, q3;
	QueueInit(&q1);
	QueueInit(&q2);
	QueueInit(&q3);

	pthread_mutex_t mtx_q1, mtx_q2, mtx_q3;
	pthread_mutex_init(&mtx_q1, NULL);
	pthread_mutex_init(&mtx_q2, NULL);
	pthread_mutex_init(&mtx_q3, NULL);

	pthread_cond_t cond_q1, cond_q2, cond_q3;
	pthread_cond_init(&cond_q1, NULL);
	pthread_cond_init(&cond_q2, NULL);
	pthread_cond_init(&cond_q3, NULL);

	int q1_sz = 0, q2_sz = 0, q3_sz = 0;
	Q_sync_t sync_p, sync_A, sync_B, sync_C;
	init_QSync(&sync_p, NULL, &q1,  NULL,    &mtx_q1, NULL,     &cond_q1, NULL,   &q1_sz);
	init_QSync(&sync_A, &q1,  &q2,  &mtx_q1, &mtx_q2, &cond_q1, &cond_q2, &q1_sz, &q2_sz);
	init_QSync(&sync_B, &q2,  &q3,  &mtx_q2, &mtx_q3, &cond_q2, &cond_q3, &q2_sz, &q3_sz);
	init_QSync(&sync_C, &q3,  NULL, &mtx_q3, NULL, 	  &cond_q3, NULL, 	  &q3_sz, NULL);


	

	// for(int i=0; i < NUM; ++i){

	// 	QueueGet(&q, &request);
	// 	printf("Agarre %d\n", request.r_id);
	// 	QueueMonitor(&q);
	// }
	pthread_t th_producer, th_A, th_B[NUM_THREADS], th_C;

	pthread_create(&th_producer, NULL, thread_producer, (void*) &sync_p);
	pthread_create(&th_A, 		 NULL, thread_A, 		(void*) &sync_A);
	pthread_create(&(th_B[0]), 	 NULL, thread_A, 		(void*) &sync_B);
	pthread_create(&th_C, 		 NULL, thread_C, 		(void*) &sync_C);




	pthread_join(th_producer, 	NULL);
	pthread_join(th_A, 			NULL);
	pthread_join(th_B[0], 		NULL);
	pthread_join(th_C, 			NULL);


	pthread_cond_destroy(&cond_q1);
	pthread_cond_destroy(&cond_q2);
	pthread_cond_destroy(&cond_q3);
	pthread_mutex_destroy(&mtx_q1);
	pthread_mutex_destroy(&mtx_q2);
	pthread_mutex_destroy(&mtx_q3);
	QueueDestroy(&q1);
	QueueDestroy(&q2);
	QueueDestroy(&q3);

	return 0;
}