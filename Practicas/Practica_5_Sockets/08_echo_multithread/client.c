#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "Socket.h"

#define BUFFER_SZ 256
// #define PORT 6900
#define NUM_ECHOS 10

void error(const char* msg){
	perror(msg);
	exit(2);
}


int main(int argc, char *argv[]){

	// ------------ Declaro variables ------------
	int sockfd, n;

	struct sockaddr_in serv_addr;
	socklen_t addr_len = sizeof(serv_addr);
	char buffer[BUFFER_SZ];

	if(argc != 3){
		fprintf(stderr, "Usage %s address port\n", argv[0]);
		exit(1);
	}


	// ------------ Configuro el socket cliente ------------
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sockfd < 0){
		error("Error opening socket\n");
	}

	// Seteo family, paso addr de string a network Order y el puerto
	memset((void*) &serv_addr, 0, addr_len);
	serv_addr.sin_family = AF_INET;
	// serv_addr.sin_addr.s_addr 	= htonl(inet_addr(argv[2]));
	if( inet_aton(argv[1], &serv_addr.sin_addr) == 0){
		error("Invalid Address");
	}
	serv_addr.sin_port = htons(atoi(argv[2]));


	if(connect(sockfd, (struct sockaddr *) &serv_addr, addr_len) < 0){
		error("Error on connect");
	}


	// ------------ Protocolo o lo que sea ------------
	bzero((void*) buffer, BUFFER_SZ);
	strcpy(buffer, "Dummy msg 0");
	printf("%s\n", buffer);
	for(int i=0; i < NUM_ECHOS; i++){

		buffer[strlen(buffer)-1] = (char) ('0'+i);

		n = write_socket(sockfd, buffer, BUFFER_SZ);
		if(n < 0){
			error("Error writing to socket");
		}
		// n = read(sockfd, buffer, BUFFER_SZ);
		n = read_socket(sockfd, buffer, BUFFER_SZ);
		if(n < 0){
			error("Error reading from socket");
		}
		printf("Echo: %s\n", buffer);
		sleep(1);
	}

	// ------------ Libero recursos ------------
	write(sockfd, "end", 4);
	close(sockfd);
	
	return 0;
}