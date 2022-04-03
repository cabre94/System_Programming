#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tftp_packets.h"



int main(int argc, char* argv[]){

	char *filename 	= "hola_mundo.txt";
	char *data 		= "abcdefghijk";
	// char *error_msg = "error_msg";

	// char *packet = WRQ_create(filename);
	char *rrq = RRQ_create(filename);
	char *wrq = WRQ_create(filename);

	char *data_p 	= DATA_create(19, data);
	// char *ack 	= ACK_create(21);
	// char *err 	= ERROR_create(23, error_msg);

	printf("El WRQ es: %s y el largo es %ld\n", rrq, strlen(rrq));
	printf("El RRQ es: %s y el largo es %ld\n", wrq, strlen(wrq));
	printf("El DAT es: %s y el largo es %ld\n", data_p, strlen(data_p));
	// printf("El ACK es: %s y el largo es %ld\n", ack, strlen(ack));
	// printf("El ERR es: %s y el largo es %ld\n", err, strlen(err));

	char filename2[64];
	strcpy(filename2, "server-");
	strcat(filename2, rrq+2);
	printf("filename: %s\n", filename2);

	free(rrq);
	free(wrq);
	free(data_p);
	// free(ack);
	// free(err);

	return 0;
}