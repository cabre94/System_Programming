#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>

#define NUM_THREADS 50

typedef struct _thread_data_t{
	int tid;
	double stuff;
	/* data */
} thread_data_t;

// shared data between threads
double shared_x;
pthread_mutex_t lock_x;


void* thr_func(void* arg){

	thread_data_t *data = (thread_data_t*) arg;

	printf("Hello from thr_func, thread %d\n", data->tid);

	// get mutex before modifying and printing shared x
	// pthread_mutex_lock(&lock_x);
	// shared_x += data->stuff;
	// printf("x = %lf\n", shared_x);
	// pthread_mutex_unlock(&lock_x);

	// Usar la printf es caro asi que esto es mejor
	double val;
	pthread_mutex_lock(&lock_x);
	shared_x += data->stuff;
	val = shared_x;
	pthread_mutex_unlock(&lock_x);
	printf("x = %lf\n", val);

	pthread_exit(NULL);
}

int main(){

	pthread_t threads[NUM_THREADS];

	int rc;
	// create a thread_data aargument array
	thread_data_t thr_data[NUM_THREADS];

	// initilized shared data
	shared_x = 0;

	pthread_mutex_init(&lock_x, NULL);

	// create threads
	for(int i = 0; i < NUM_THREADS; ++i){
		thr_data[i].tid = i;
		thr_data[i].stuff = i * NUM_THREADS;

		if( rc = pthread_create(&threads[i], NULL, thr_func, (void*) &thr_data[i])){
			fprintf(stderr ,"ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	// block until all thread complete
	for(int i=0; i < NUM_THREADS; ++i){
		pthread_join(threads[i], NULL);
	}

	// return 0;

	// Last thing that main should do
	printf("Main program complete\n");
	pthread_exit(NULL);

}