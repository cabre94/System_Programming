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

    Queue_t* pQ = create_shm("/memqueque", 10);

	if(pQ == NULL){
		return 1;
	}

	detach_shm(pQ);
	
    return 0;
}

