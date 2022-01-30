#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mq_utils.h"



int main(int argc, char* argv[]){

	if(argc != 4){
		printf("Error en la cantidad de inputs: mq_name\tmsg_SZ\tmax_msg\n");
		printf("Pase %d\n", argc);
		return -1;
	}

	char filename[32];
	strcpy(filename, argv[1]);
	filename[32] = '\0';

	long MSG_SIZE 	= strtol(argv[2], NULL, 10);
	long MSG_MAX 	= strtol(argv[3], NULL, 10);

	

	mqd_t mqd = create_mq(filename, MSG_SIZE, MSG_MAX);

	detach_mq(mqd);

	return 0;

}