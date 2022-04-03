#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

// #include "Socket.h"

#define BUFFER_SZ 256
#define PORT 6900

void error(const char* msg){
	perror(msg);
	exit(2);
}

typedef struct{
	uint32_t id;
	char c[16];
} data_t;


int main(int argc, char *argv[]){
	// printf("size of double: %ld\n", sizeof(double));
	// printf("size of double*: %ld\n", sizeof(double*));
	// printf("size of uint32_t: %ld\n", sizeof(uint32_t));
	// printf("size of uint32_t*: %ld\n", sizeof(uint32_t*));

	// ------------ Declaro variables ------------
	int sockfd;

	struct sockaddr_in serv_addr;
	socklen_t addr_len = sizeof(serv_addr);
	// char buffer[BUFFER_SZ];

	if(argc != 3){
		fprintf(stderr, "Usage %s address num_packets\n", argv[0]);
		exit(1);
	}


	// ------------ Configuro el socket cliente ------------
	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sockfd < 0){
		error("Error opening socket\n");
	}

	// Seteo family, paso addr de string a network Order y el puerto
	bzero((void*) &serv_addr, addr_len);
	serv_addr.sin_family = AF_INET;
	// serv_addr.sin_addr.s_addr 	= htonl(inet_addr(argv[2]));
	if( inet_aton(argv[1], &serv_addr.sin_addr) == 0){
		error("Invalid Address");
	}
	serv_addr.sin_port = htons(PORT);


	// ------------ Protocolo o lo que sea ------------
	data_t packet = {0, "messege"};
	size_t pck_sz = sizeof(packet);
	
	uint32_t num_packets = (uint32_t) atoi(argv[2]);

	while(packet.id < num_packets){
		if(sendto(sockfd, (void*) &packet, pck_sz, 0, (struct sockaddr *) &serv_addr, addr_len) == -1){
			error("sendto()");
		}
		packet.id++;
	}


	// ------------ Libero recursos ------------
	close(sockfd);
	
	return 0;
}