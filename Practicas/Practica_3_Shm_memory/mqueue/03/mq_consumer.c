#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mq_utils.h"


int main(int argc, char** argv){

	// Paso los nombres como argumentos
	mqd_t* p_mqd = (mqd_t *) malloc(sizeof(mqd_t) * (argc-1));


	int ret;
	int max_fd = 0;
	for(int i=0; i < (argc-1); ++i){
		ret = attach_mq(argv[i+1]);
		if(-1 == (int) ret){
			perror("Error attaching");
		}
		p_mqd[i] = ret;
		if(ret >= max_fd)
			max_fd = ret;
	}

	// Ahora hay que hacer el select del orto
	ssize_t n_bytes;		// bytes leidos
	int n_fd;
	fd_set rfds;			// files descriptors de lectura
	struct timeval tv;
	char buff[64];

	tv.tv_sec 	= 1;
	tv.tv_usec 	= 0;

	while(1){
		FD_ZERO(&rfds);
		FD_SET(STDIN_FILENO, &rfds);
		for(int i=0; i < (argc-1); ++i){
			FD_SET(p_mqd[i], &rfds);
		}

		n_fd = select(max_fd+1, &rfds, NULL, NULL, &tv);

		if(n_fd == 0){
			printf("Ninguna mqueue para leer. Enter para terminar.\n");
			sleep(5);	// Espero un seg 1 medio
			continue;
		}else if (n_fd == -1){
			perror("Error select");
			// Aca habria que cerrar los mq que se abrieron
			exit(EXIT_FAILURE);
		}

		if(FD_ISSET(STDIN_FILENO, &rfds)){
			printf("Terminaos\n");
			break;	// Si mando algo por terminal, corto.
		}

		// Leo todo el resto de mq
		for(int i=0; i < (argc-1); ++i){
			if(FD_ISSET(p_mqd[i], &rfds)){
				n_bytes = mq_receive(p_mqd[i], buff, sizeof(buff), NULL);
				if(n_bytes < 0){
					perror("mq_received");
					// cerrar todo
					exit(EXIT_FAILURE);
				}
				printf("message #%d -> %s\n",  i, buff);
			}
		}
	}


	for(int i=0; i < (argc-1); ++i){
		detach_mq(p_mqd[i]);
	}

	free(p_mqd);

	printf("Terminando...");
	sleep(1);

	return 0;
}