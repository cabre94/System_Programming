#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_THREADS 15

void* printHello(void* threadid){

	long tid;
	tid = (long) threadid;

	usleep(500);
	printf("Hello Wolrd!, thread id: %ld\n", tid);
	pthread_exit(NULL);
}

int main(){

	pthread_t threads[NUM_THREADS];
	int rc;
	long t;

	for(t = 0; t < NUM_THREADS; ++t){
		printf("In main: creating thread %ld\n", t);
		rc = pthread_create(&threads[t], NULL, printHello, (void*) t);

		if(rc){
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	// return 0;

	// Last thing that main should do
	pthread_exit(NULL);



	return 0;
}