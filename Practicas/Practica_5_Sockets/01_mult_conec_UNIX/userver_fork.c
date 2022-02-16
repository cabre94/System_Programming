#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
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


void manage_socket(int newsockfd){
	char buffer[BUFFER_SZ];
	bzero(buffer, BUFFER_SZ);

	if(read_socket(newsockfd, buffer, BUFFER_SZ-1) < 0)
		error("Error reading from socket\n");
	printf("Here is the messege: %s\n", buffer);
	if(write_socket(newsockfd, "I got your messege", BUFFER_SZ) < 0)
		error("Error writing to socket\n");

	close(newsockfd);
}

int main(int argc, const char* argv[]){

	int sockfd, newsockfd;

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

	pid_t pid;
	int counter = 0;	// Voy a permitir 5 conexiones
	while(counter != 5){
	
		if( (newsockfd = accept(sockfd, NULL, NULL)) < 0)
			error("Error on accept");

		pid = fork();
		if(pid == 0){
			manage_socket(newsockfd);
			close(sockfd);
			return 0;
		}else if(pid != -1)
			close(newsockfd); // Cierro socket, el hijo tiene una copia, creo...
		else
			error("fork");
		
		counter++;
	}

	for(int i = 0; i < counter; ++i)
		wait(NULL);

	close(sockfd);

	return 0;
}