#ifndef QUEQUE_H
#define QUEQUE_H

// #pragma once

#include <semaphore.h>

#define Q_SZ 8

typedef struct{
    sem_t mtx_sync;			// sinc de acceso, mutex
    sem_t sem_aval;			// cant. de entradas libres
    sem_t sem_used;			// bloqueo para proc. productores
    int elem[Q_SZ];
    unsigned int idx_put;
    unsigned int idx_get;
} Queue_t;

// Inicializa (debe residir en un segmento de shared memory)
void QueueInit(Queue_t *pQ);
// Desstruye el contenedor, liberando recursos
void QueueDestroy(Queue_t *pQ);
// Agrega un Nuevo elemento. Bloquea si no hay espacio
void QueuePut(Queue_t *pQ, int elem);
// Remueve y retorna un elemento, bloquea si no hay elementos
int QueueGet(Queue_t *pQ);
// recupera la cantidad de elementos en la cola
int QueueSize(Queue_t *pQ);
// monitorea la cola
void QueueMonitor(Queue_t *pQ);

#endif // !QUEQUE_H



