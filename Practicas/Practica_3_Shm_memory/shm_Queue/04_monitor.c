#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/wait.h>

#include "Queque.h"
#include "shmQueue.h"


int main(int argc, const char *argv[]){
	
    Queue_t* pQ = attach_shm("/memqueque");

	QueueMonitor(pQ);

	detach_shm(pQ);
	
    return 0;
}

