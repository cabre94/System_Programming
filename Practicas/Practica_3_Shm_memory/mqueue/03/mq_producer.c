#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mq_utils.h"


int main(int argc, char** argv){

	if(argc != 3){
		printf("Error en la cantidad de inputs: mq_name\tn_items\n");
		return -1;
	}

	char filename[64];
	strcpy(filename, argv[1]);
	filename[63] = '\0';

	long n_items = strtol(argv[2], NULL, 10);

	
	mqd_t mqd = attach_mq(filename);

	send_messeges(mqd, n_items, filename);

	detach_mq(mqd);

	return 0;
}