#ifndef QUEUE_H
#define QUEUE_H

// #pragma once

#include <pthread.h>
#include <WorkUnit.h>

#define Q_SZ 10

typedef struct{
    pthread_mutex_t mtx_sync;		// sinc de acceso, mutex
    pthread_cond_t  get_ready;		// cant. de entradas libres
    pthread_cond_t 	put_ready;		// condicion para hacer put en la queue
    WorkUnit_t units[Q_SZ];
    unsigned long idx_put;
    unsigned long idx_get;
} Queue_t;

// Inicializa (debe residir en un segmento de shared memory)
int QueueInit(Queue_t *pQ);
// Desstruye el contenedor, liberando recursos
int QueueDestroy(Queue_t *pQ);
// Agrega un Nuevo elemento. Bloquea si no hay espacio
int QueuePut(Queue_t *pQ, WorkUnit_t* w_unit);
// Remueve y guarda un elemento, bloquea si no hay elementos
int QueueGet(Queue_t *pQ, WorkUnit_t* w_unit);
// recupera la cantidad de elementos en la cola
unsigned long QueueSize(Queue_t *pQ);
// monitorea la cola
// void QueueMonitor(Queue_t *pQ);

#endif // !QUEUE_H



