#ifndef QUEUE_H
#define QUEUE_H

// #pragma once

#include <pthread.h>

// #include "WorkUnit.h"
#include "UDTs.h"

#define Q_SZ 10
#define QUEUE_TIMEOUT 5

// typedef struct WorkUnit_t;
struct WorkUnit_t;


typedef struct{
    pthread_mutex_t mtx_sync;		// sinc de acceso, mutex
    pthread_cond_t  get_ready;		// cant. de entradas libres
    pthread_cond_t 	put_ready;		// condicion para hacer put en la queue
    Req_t units[Q_SZ];
    long idx_put;
    long idx_get;
	unsigned long size;
} Queue_t;

// Inicializa (debe residir en un segmento de shared memory)
int QueueInit(Queue_t *pQ);
// Desstruye el contenedor, liberando recursos
int QueueDestroy(Queue_t *pQ);
// Agrega un Nuevo elemento. Bloquea si no hay espacio
int QueuePut(Queue_t *pQ, Req_t* pReq);
// Remueve y guarda un elemento, bloquea si no hay elementos
int QueueGet(Queue_t *pQ, Req_t* pReq);
// recupera la cantidad de elementos en la cola
unsigned long QueueSize(Queue_t *pQ);
// monitorea la cola
int QueueMonitor(Queue_t *pQ, char prefix);

#endif // !QUEUE_H



