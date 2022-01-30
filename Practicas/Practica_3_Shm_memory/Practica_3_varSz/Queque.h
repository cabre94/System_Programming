#ifndef QUEQUE_H
#define QUEQUE_H

// #pragma once

#include <semaphore.h>

typedef struct Queue_str Queue_t;

// Inicializa (debe residir en un segmento de shared memory)
void QueueInit(Queue_t *pQ, unsigned int num_elems);
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

size_t getQueueSize(unsigned num_elems);
unsigned getQueueNumElems(const Queue_t *pQ);

#endif // !QUEQUE_H



