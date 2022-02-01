#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <errno.h>


#define MAXNTHREADS 100

void *reader(void *), *writer(void *);

int nloop = 1000, nreaders = 50, nwriters = 40;

struct{
	pthread_rwlock_t 	rwlock;
	pthread_mutex_t 	rcountlock;
	int 				nreaders;
	int 				nwriters;
} shared = {PTHREAD_RWLOCK_INITIALIZER, PTHREAD_MUTEX_INITIALIZER};


int main(){

	pthread_t tid_readers[MAXNTHREADS], tid_writers[MAXNTHREADS];
	int rc;

	pthread_mutex_init(&shared.rcountlock, NULL);
	pthread_rwlock_init(&shared.rwlock, NULL);

	// create readers and writers threads
	for(int i = 0; i < nreaders; ++i){
		if( (rc = pthread_create(&tid_readers[i], NULL, reader, NULL)) ){
			fprintf(stderr, "error: thread_create readers, rc: %d\n", rc);
			return EXIT_FAILURE;
		}
	}
	printf("readers created\n");

	for(int i = 0; i < nwriters; ++i){
		if( (rc = pthread_create(&tid_writers[i], NULL, writer, NULL)) ){
			fprintf(stderr, "error: thread_create readers, rc: %d\n", rc);
			return EXIT_FAILURE;
		}
	}
	printf("writers created\n");

	// wait for all threads to complete
	for(int i =0; i < nreaders; ++i){
		pthread_join(tid_readers[i], NULL);
	}
	printf("Readers done\n");

	for(int i =0; i < nwriters; ++i){
		pthread_join(tid_writers[i], NULL);
	}
	printf("Writers done\n");

	return 0;
}


void *reader(void *arg){
	int i;

	sleep(1);

	for(int i = 0; i < nloop; ++i){

		if(pthread_rwlock_rdlock(&shared.rwlock) < 0){
			perror("rdlock");
		}

		if(pthread_mutex_lock(&shared.rcountlock) < 0){
			perror("mutex_lock");
		}

		shared.nreaders++;	// shared by all readers, must protect
		if(pthread_mutex_unlock(&shared.rcountlock) < 0){
			perror("mutex_unlock");
		}

		if(shared.nwriters > 0){
			fprintf(stderr, "reader: %d writers found\n", shared.nwriters);
			exit(1);
		}

		if(pthread_mutex_lock(&shared.rcountlock) < 0){
			perror("mutex_lock");
		}
		shared.nreaders--;	// shared by all readers, must protect
		if(pthread_mutex_unlock(&shared.rcountlock) < 0){
			perror("mutex_lock");
		}

		if(pthread_rwlock_unlock(&shared.rwlock) < 0){
			perror("rd_unlock");
		}
	}

	return NULL;
}

void *writer(void *arg){

}


