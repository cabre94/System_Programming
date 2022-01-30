#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




int main(int argc, char** argv){

	if(argc != 4){
		printf("Error en la cantidad de inputs: mq_name\tmsg_SZ\tmax_msg\n");
		return -1;
	}

	char filename[64];
	strcpy(filename, argv[1]);
	filename[63] = '\0';

	long MSG_SIZE 	= strtol(argv[2], NULL, 10);
	long MSG_MAX 	= strtol(argv[3], NULL, 10);

	// printf("Arg 1: %s\n", argv[1]);
	// printf("Arg 2: %ld\n", MSG_SIZE);
	// printf("Arg 3: %ld\n", MSG_MAX);

	mqd_t mqd;
    struct mq_attr attr;        /* buffer for stat info */

	attr.mq_msgsize = MSG_SIZE;
    attr.mq_maxmsg = MSG_MAX;


	mqd = mq_open("/test", O_RDWR | O_CREAT | O_EXCL, 0600, &attr);

	if(-1 == (int) mqd){
		perror("Erro mq_open");
		exit(EXIT_FAILURE);
	}




}