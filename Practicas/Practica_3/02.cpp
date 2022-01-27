#include <unistd.h>
#include <fcntl.h>
#include <cassert>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <iostream>
// #include <semaphore>

#define Q_SZ 8

class Queue_t{
public:
	Queue_t();

	Queue_t(Queue_t &&) 				= delete;
	Queue_t(const Queue_t &) 			= delete;
	Queue_t &operator=(Queue_t &&) 		= delete;
	Queue_t &operator=(const Queue_t &) = delete;
	
	~Queue_t();

	void QueuePut(int elem);
	int QueueGet();
	int QueueSize();

	void QueueMonitor();
private:
	// const int Q_SZ;
	sem_t mtx_sync;			// sinc de acceso, mutex
    sem_t sem_aval;			// cant. de entradas libres
    sem_t sem_used;			// bloqueo para proc. productores
    int elem[Q_SZ];
    unsigned int idx_put;
    unsigned int idx_get;
};






int main(int argc, const char *argv[])
{
    // hago el mapeo al vector pQ
    Queue_t *pQ = (Queue_t *) mmap(NULL, sizeof(Queue_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    //inicializo la la cola

    int pid, i;

    // forkeo un hijo
    pid = fork();
    switch (pid)
    {
    case -1:
        perror("fork child 1");
    case 0:
        //child - agrega elementos
        for (i = 0; i < 100; i++)
        {
            if ((i % 10) == 0)
            {
                //cada 10 quiero monitorear la cosa
                pQ->QueueMonitor();
            }
            // agrego un elemento
            pQ->QueuePut(i);
        }
        // el hijo termina aca
        return 0;
        break;
    default:
        //father - saca elementos
        for (i = 0; i < 100; i++)
        {
            pQ->QueueGet();
        }
        break;
    }
    // espero a que termine el hijo
    wait(NULL);
    printf("All processes finished\n");
    // monitoreo, deberia dar 0 elementos
    pQ->QueueMonitor();
    // vacio semaforos de la cola
    // QueueDestroy(pQ);
    // destruyo el puntero a la cola compartido
    munmap(pQ, sizeof(Queue_t));

    return 0;
}




Queue_t::Queue_t(){
	idx_put = 0;
	idx_get = 0;

	// Me canse del perror
	assert(sem_init(&mtx_sync, 1, 1) 	!= -1);
	assert(sem_init(&sem_aval, 1, Q_SZ) != -1);
	assert(sem_init(&sem_used, 1, 0) 	!= -1);
}

Queue_t::~Queue_t(){
	assert(sem_destroy(&mtx_sync) != -1);
	assert(sem_destroy(&sem_aval) != -1);
	assert(sem_destroy(&sem_used) != -1);
}

void Queue_t::QueuePut(int elem_){
	assert(sem_wait(&sem_aval) != -1);
	assert(sem_wait(&mtx_sync) != -1); // Por si alguien lo esta  usando

	// Hay lugar y nadie lo esta usando
	elem[idx_put % Q_SZ] = elem_;
	idx_put++;

	assert(sem_post(&mtx_sync) != -1); // Lo libero
	assert(sem_post(&sem_used) != -1);
}

int Queue_t::QueueGet(){
	assert(sem_wait(&sem_used) != -1);
	assert(sem_wait(&mtx_sync) != -1); // Por si alguien lo esta  usando

	// Hay elementos y nadie lo esta usando
	int value = elem[idx_get % Q_SZ];
	idx_get++;

	assert(sem_post(&mtx_sync) != -1); // Lo libero
	assert(sem_post(&sem_aval) != -1);

	return value;
}

int Queue_t::QueueSize(){
	assert(sem_wait(&mtx_sync) != -1); // Por si alguien lo esta  usando

	// Hay elementos y nadie lo esta usando
	int sz = idx_put - idx_get;

	assert(sem_post(&mtx_sync) != -1); // Lo libero

	return sz;
}

void Queue_t::QueueMonitor(){
	assert(sem_wait(&mtx_sync) != -1); // Por si alguien lo esta  usando

	// Hay elementos y nadie lo esta usando
	int sz = idx_put - idx_get;
	printf("-> ");
	for(int i = 0; i < sz; i++){
        printf("%d -> ", elem[(idx_get + i) % Q_SZ]);
    }
	putchar('\n');

	assert(sem_post(&mtx_sync) != -1); // Lo libero
}