#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Socket.h"

#define BUFFER_SZ 256

void error(const char* msg){
	perror(msg);
	exit(1);
}

int main(int argc, const char* argv[]){

	int sockfd, newsockfd, n;

	char buffer[BUFFER_SZ];
	struct sockaddr_un serv_addr;

	if(argc != 2){
		fprintf(stderr, "Error: no path provided\n");
		exit(1);
	}

	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockfd < 0){
		error("Error opening socket");
	}

	bzero((void*) &serv_addr, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, argv[1]);

	unlink(argv[1]); 	/* Como el socket es un file, se hace un unlink para que
						si ese file ya existia hacerlo volar */

	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr_un)) < 0){
		perror("Error on binding");
	}

	listen(sockfd, 5);

	newsockfd = accept(sockfd, NULL, NULL);	/*Queda bloqueado hasta que alguien
											se conecte*/

	if(newsockfd < 0){
		error("Error on accept");
	}

	bzero(buffer, BUFFER_SZ);

	n = read(newsockfd, buffer, BUFFER_SZ-1);
	if(n < 0){
		error("Error reading from socket\n");
	}

	printf("Here is the messege: %s\n", buffer);

	n = write(newsockfd, "I got your messege", 18);
	if(n < 0){
		error("Error writing to socket\n");
	}

	close(newsockfd);
	close(sockfd);

	return 0;
}