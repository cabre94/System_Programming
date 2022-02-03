#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <errno.h>


#define  NUM_THREADS 3
#define  TCOUNT 10 
#define  COUNT_LIMIT 12

int count = 0;
pthread_mutex_t count_mutex;
pthread_cond_t count_threshold_cv;


typedef struct my_sem{
	unsigned int 	counter;
	pthread_mutex_t mutex;
	pthread_cond_t 	cv_ready;
} my_sem_t;

int init_my_sem(my_sem_t* sem, unsigned int value){
	int ret_v;

	// inicializo el mutex
	if((ret_v = pthread_mutex_init(&sem->mutex, NULL)) != 0){
		return ret_v;
	}

	// Inicializo la cond_v
	if((ret_v = pthread_cond_init(&sem->cv_ready, NULL)) != 0){
		pthread_mutex_destroy(&sem->mutex); // falla si no es mutex (hubiera fallado antes) o algun lo usa
		return ret_v; // ¿?
	}

	sem->counter = value;

	return 0;
}

int destroy_my_sem(my_sem_t* sem){
	int ret_v;
	
	// Libero el mutex
	if((ret_v = pthread_mutex_destroy(&sem->mutex)) != 0 ){
		pthread_cond_destroy(&sem->cv_ready); // No se bien como manejar esto
		return ret_v;
	}

	// Libero la cond_v
	if((ret_v = pthread_cond_destroy(&sem->cv_ready)) != 0 ){
		return ret_v;
	}

	return 0;
}


int my_sem_post(my_sem_t* sem){
	int ret_val;
	// Me agarro el mutex para aumentar el contador
	if((ret_val = pthread_mutex_lock(&sem->mutex)) != 0){
		return ret_val;
	}

	sem->counter++;

	// No se si tengo que mandar la patadita siempre o solo cuando counter es 1
	if(sem->counter == 1){
		pthread_cond_signal(&sem->cv_ready);
	}

	// desbloqueo el mutex
	if((ret_val = pthread_mutex_unlock(&sem->mutex)) != 0){
		return ret_val;
	}

	return 0;
}

int my_sem_wait(my_sem_t* sem){
	int ret_val;
	// Me agarro el mutex para aumentar el contador
	if((ret_val = pthread_mutex_lock(&sem->mutex)) != 0){
		return ret_val;
	}

	// Aca espero hasta que alguien haya aumentado el contador
	while(sem->counter == 0){
		pthread_cond_wait(&sem->cv_ready, &sem->mutex);
	}

	sem->counter--;
	// desbloqueo el mutex
	if((ret_val = pthread_mutex_unlock(&sem->mutex)) != 0){
		return ret_val;
	}

	return 0;
}

typedef struct {
	my_sem_t lock;
	int contador;
} Data_t;

void* increse(void* arg){
	Data_t* data = (Data_t*) arg;
	usleep((rand() % 100) * 3000);

	my_sem_wait(&data->lock);
	data->contador++;
	printf("Increse: %d\n", data->contador);
	my_sem_post(&data->lock);

	pthread_exit(NULL);
}

void* decrese(void* arg){
	Data_t* data = (Data_t*) arg;
	// usleep((rand() % 100) * 3000);

	my_sem_wait(&data->lock);
	data->contador--;
	printf("Decrese: %d\n", data->contador);
	my_sem_post(&data->lock);

	pthread_exit(NULL);
}


int main(){

	int rc;
	Data_t data;

	init_my_sem(&data.lock, 1);
	data.contador = 1;


	pthread_t inc[10];
	pthread_t dec[10];

	// Mando los thread a incrementar y decrementar
	for(int i = 0; i < 10; ++i){
		if( rc = pthread_create(&dec[i], NULL, decrese, (void*) &data)){
			fprintf(stderr ,"ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
		if( rc = pthread_create(&inc[i], NULL, increse, (void*) &data)){
			fprintf(stderr ,"ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}


	for(int i=0; i < 10; ++i){
		pthread_join(inc[i], NULL);
		pthread_join(dec[i], NULL);
	}



	destroy_my_sem(&data.lock);



	return 0;
}