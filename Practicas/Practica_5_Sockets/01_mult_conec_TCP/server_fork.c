#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

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

	// ------------ Declaro variables ------------
	int sockfd, newsockfd;

	struct sockaddr_in serv_addr, cli_addr;
	socklen_t addr_len = sizeof(serv_addr);
	// char buffer[BUFFER_SZ];

	if(argc != 2){
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}


	// ------------ Configuro el socket cliente ------------
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sockfd < 0){
		error("Error opening socket");
	}

	bzero((void*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family 		= AF_INET;
	serv_addr.sin_addr.s_addr 	= INADDR_ANY;
	serv_addr.sin_port 			= htons(atoi(argv[1]));

	int val;	// No tengo muy claro que pasa con el val
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	if(bind(sockfd, (struct sockaddr *) &serv_addr, addr_len) < 0){
		perror("Error on binding");
	}

	listen(sockfd, 5);


	// ------------ Protocolo o lo que sea ------------
	pid_t pid;
	int counter = 0;	// Voy a permitir 5 conexiones
	while(counter != 5){
	
		if((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &addr_len)) < 0)
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