#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define NUM_THREADS 10

void *busy(void *arg){
	long t_id = (long) arg;

	for(int i=0; i < 5; ++i){
		printf("Thread %ld working\n", t_id);
		sleep(1);
	}

	pthread_exit(NULL);
}

int main(){

	pthread_t threads[2];

	pthread_attr_t attr;
	pthread_attr_init(&attr);

	size_t stack_sz;
	int err_value;

	// Lanzo el primer thread con valores por default
	pthread_create(&threads[0], &attr, busy, (void*) 0L);

	if((err_value = pthread_attr_getstacksize(&attr, &stack_sz)) < 0){
		printf("getstacksize error #%d\n", err_value);
	}
	printf("Tamaño del primer thread: %ld\n", stack_sz);

	// Cambio el tamaño 
	if((err_value = pthread_attr_setstacksize(&attr, 2*stack_sz)) < 0){
		printf("setstacksize error #%d\n", err_value);
	}
	
	// Lanzo el seguno thread con valores con el soble de tamaño
	pthread_create(&threads[1], &attr, busy, (void*) 1L);
	if((err_value = pthread_attr_getstacksize(&attr, &stack_sz)) < 0){
		printf("getstacksize error #%d\n", err_value);
	}
	printf("Tamaño del segundo thread: %ld\n", stack_sz);



	pthread_exit(NULL);

}