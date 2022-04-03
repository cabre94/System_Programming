#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "Socket.h"

#define BUFFER_SZ 256
#define NUM_THREADS 5

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

void* manage_sck_thread(void* arg){
	manage_socket( *((int*) arg) );
	pthread_exit(NULL);
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
	// Configuro los atributos de los threads
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	pthread_t threads[NUM_THREADS];
	int counter = 0;	// Voy a permitir 5 conexiones
	while(counter != NUM_THREADS){

		if((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &addr_len)) < 0)
			error("Error on accept");
		
		if(pthread_create(&threads[counter], &attr, manage_sck_thread, (void*) &newsockfd) != 0){
			perror("pthread_create");
		}


		counter++;
	}

	pthread_attr_destroy(&attr);
	close(sockfd);

	pthread_exit(NULL);
	// return 0;
}