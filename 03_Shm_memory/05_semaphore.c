#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <errno.h>

#define SH_BUF_SIZE 0x1000

typedef struct {
	sem_t lock;
	char buff[SH_BUF_SIZE];
} Data_t;

void init_Data(Data_t *pDat){
	sem_init(&pDat->lock, 1, 1);
}

void lock_Data(Data_t *pDat){
	while(sem_wait(&pDat->lock) == -1 && errno == EINTR)
		;
}

void unlock_Data(Data_t *pDat){
	sem_post(&pDat->lock);
}

void hijo(Data_t *pDat){
	lock_Data(pDat);
	// bla bla
	unlock_Data(pDat);
}

void padre(Data_t *pDat){
	lock_Data(pDat);
	// bla bla
	unlock_Data(pDat);
}


int main(int argc, char const *argv[]){

	Data_t *p = mmap(NULL, sizeof(Data_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	assert(p != MAP_FAILED);

	init_Data(p);

	pid_t pid = fork();
	if(pid == 0){
		hijo(p);
	}else if(pid > 0){
		padre(p);
		wait(NULL);
	}else{
		perror("fork error");
		munmap(p, sizeof(Data_t));
		exit(EXIT_FAILURE);
	}

	munmap(p, sizeof(Data_t));

	return 0;
}
