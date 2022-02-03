#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

#define NUM_THREADS 5000

typedef struct myMutex{
	sem_t mtx;
} myMutex_t;


int myMutex_init(myMutex_t* pMutex){
	int error_id;
	if( (error_id = sem_init(&pMutex->mtx, 0, 1)) == -1){ // pshared en 0 porque va a ser compartido entre threads
		perror("Error in myMutex_destroy calling sem_init");
	}
	return error_id;
}

int myMutex_destroy(myMutex_t *pMutex){
	int error_id;
	if( (error_id = sem_destroy(&pMutex->mtx)) == -1){
		perror("Error in myMutex_destroy calling sem_destroy");
	}
	return error_id;
}

int myMutex_lock(myMutex_t* pMutex){
	int error_id;
	while((error_id = sem_wait(&pMutex->mtx)) == -1 && errno == EINTR)
		;

	return error_id;
}

int myMutex_unlock(myMutex_t *pMutex){
	int error_id;
	/* Voy a asumir que el usuario lo usa bien y no va a hacer un unlock sobre
	un mutex desbloqueado. */
	if( (error_id = sem_post(&pMutex->mtx)) == -1 ){
		perror("Error in myMutex_unlock calling sem_post ");
	}

	return error_id;
}


/* Esta estructura es la data que le paso al thread. Tiene un contador y un 
mutex que protege el acceso.*/

typedef struct _thread_data_t{
	long thread_id;
	long *counter;
	myMutex_t *mutex;
} thread_data_t;

void *thread_fun(void* arg){

	thread_data_t *data = (thread_data_t*) arg;

	// printf("Hello from thr_func, thread %ld\n", data->thread_id);

	myMutex_lock(data->mutex);
	(*data->counter)++;
	// long val = *(data->counter);
	myMutex_unlock(data->mutex);

	// printf("x = %ld\n", val);

	pthread_exit(NULL);
}

int main(){

	pthread_t threads[NUM_THREADS];
	thread_data_t thr_data[NUM_THREADS];

	long shared_counter = 0;
	myMutex_t mutex;

	myMutex_init(&mutex);

	for(int i = 0; i < NUM_THREADS; ++i){
		thr_data[i].counter = &shared_counter;
		thr_data[i].mutex = &mutex;
		thr_data[i].thread_id = i;
	}

	int rc;
	for(int i = 0; i < NUM_THREADS; ++i){
		if( rc = pthread_create(&threads[i], NULL, thread_fun, (void*) &thr_data[i])){
			fprintf(stderr ,"ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	for(int i=0; i < NUM_THREADS; ++i){
		pthread_join(threads[i], NULL);
	}

	myMutex_destroy(&mutex);

	printf("Main program complete. Final x = %ld\n", shared_counter);
	pthread_exit(NULL);
}