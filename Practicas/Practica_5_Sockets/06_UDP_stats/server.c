#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

// #include "Socket.h"

#define BUFFER_SZ 256
#define PORT 6900
#define RECV_TIMEOUT 5

void error(const char* msg){
	perror(msg);
	exit(1);
}

int recvFromTimeout(int sock, void *buf, size_t len, struct sockaddr *addr, socklen_t *addr_len, int timeoutSecs){
	fd_set fdrs;
	FD_ZERO(&fdrs);
	FD_SET(sock, &fdrs);

	struct timeval tv;
	tv.tv_sec = timeoutSecs;
	tv.tv_usec = 0;

	int n_fd = select(sock+1, &fdrs, NULL, NULL, &tv);

	if(n_fd == -1){
		perror("Error select");
		return -1;
	}else if (n_fd == 0){
		return -2;
	}else if(FD_ISSET(sock, &fdrs)){
		return recvfrom(sock, buf, len, 0, addr, addr_len);
	}
	return -2;
}

typedef struct{
	uint32_t id;
	char c[16];
} data_t;

int main(int argc, const char* argv[]){

	// ------------ Declaro variables ------------
	int sockfd;

	struct sockaddr_in serv_addr, cli_addr;
	socklen_t addr_len = sizeof(serv_addr);
	// char buffer[BUFFER_SZ];

	// ------------ Configuro el socket cliente ------------
	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sockfd < 0){
		error("Error opening socket");
	}

	bzero((void*) &serv_addr, addr_len);
	serv_addr.sin_family 		= AF_INET;
	serv_addr.sin_addr.s_addr 	= htonl(INADDR_ANY);
	serv_addr.sin_port 			= htons(PORT);

	if(bind(sockfd, (struct sockaddr *) &serv_addr, addr_len) < 0){
		error("Error on binding");
	}

	// ------------ Protocolo o lo que sea ------------
	data_t packet;
	size_t pck_sz = sizeof(packet);
	uint32_t next = 0;
	uint32_t losted = 0;

	int rv;
	while(1){
		rv = recvFromTimeout(sockfd,(void *) &packet, pck_sz, (struct sockaddr *) &cli_addr, &addr_len, RECV_TIMEOUT);
		if(rv == -1)
			error("recvfrom()");
		if(rv == -2)
			break;

		losted += (packet.id - next);
		next = packet.id+1;

		// printf("Msg: %s. Perdidos: %0.2lf\n", packet.c, (100.0*losted)/next);
	}

	printf("Total de mensajes perdidos: %0.2lf\n", losted/(next*0.01));
	

	// ------------ Libero recursos ------------
	close(sockfd);

	return 0;
}