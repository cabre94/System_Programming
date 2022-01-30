#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "mq_utils.h"



int main(){


	pid_t pid;
	char buff[32];

	for(int i=0; i < 2; ++i){
		pid = fork();
		if(pid == 0){
			sprintf(buff, "/mq_%d", i);
			if(execl("mq_creator", "mq_creator", buff, "64", "10", (char *) NULL)){
				perror("Error execl");
				exit(EXIT_FAILURE);
			};
		}else if(pid == -1){
			perror("fork error");
		}
	}

	// acallego solo el padre
	for(int i=0; i < 2; ++i){
		pid = fork();
		if(pid == 0){
			sprintf(buff, "/mq_%d", i);
			if(execl("mq_producer", "mq_producer", buff, "5", (char *) NULL)){
				perror("Error execl");
				exit(EXIT_FAILURE);
			};
		}else if(pid == -1){
			perror("fork error");
		}
	}

	sleep(5);
	// Voy a chequear a ver si hice algo
	mqd_t mqd = attach_mq("/mq_1");
	if(mqd == -1){
		perror("attach error");
	}

	ssize_t n;
	char buff2[32];
	for(int i = 0; i < 10; i++) {
        n = mq_receive(mqd, buff2, sizeof(buff2), NULL);
		if(n < 0){
			perror("Error mq_recieve");
		}
        printf("Recibidos %ld - message #%d -> %s\n",n,  i, buff2);
    }

	detach_mq(mqd);

	destroy_mq("/mq_0");
	destroy_mq("/mq_1");

	return 0;
}
