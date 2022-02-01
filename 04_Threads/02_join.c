#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>

#define NUM_THREADS 4

void* busyWork(void* t){

	long tid;
	double result = 0.0;

	tid = (long) t;
	printf("Thread %ld starting...\n", tid);

	for(int i=0; i < 1000000; ++i){
		result = result + cos(i) * tan(i);
	}

	printf("Thread %ld done. Result = %lf\n", tid, result);
	pthread_exit((void*) t);
	// return (void*) t; // equivalente
}

int main(){

	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;
	int rc;
	long t;
	void *status;

	/* Inicializamos los atributos */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);



	for(t = 0; t < NUM_THREADS; ++t){
		printf("In main: creating thread %ld\n", t);
		rc = pthread_create(&threads[t], &attr, busyWork, (void*) t);

		if(rc){
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	/* Free atributos y se espera por los threads*/
	pthread_attr_destroy(&attr);
	
	for(t = 0; t < NUM_THREADS; ++t){
		rc = pthread_join(threads[t], &status);

		if(rc){
			printf("ERROR: return code from pthread_join() is %d\n", rc);
			exit(-1);
		}

		printf("In main: completed join thread %ld havind a status of %ld\n", t, (long) status);

	}

	// return 0;

	// Last thing that main should do
	printf("Main program complete\n");
	pthread_exit(NULL);



	return 0;
}