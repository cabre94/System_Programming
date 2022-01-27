#pragma once

#include "Queque.h"

Queue_t* create_shm(const char* memqueque_name);

Queue_t* attach_shm(const char* memqueque_name);

int detach_shm(Queue_t* pQ);

void destroy_shm(const char* memqueque_name);

