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
#include <string.h>

#include "Socket.h"

#define BUFFER_SZ 256
// #define NUM_THREADS 5
#define ACCEPT_TIMEOUT 5
#define PORT 6900
#define SERVER_IP "127.0.0.1"
// #define BROADCAST_IP "127.255.255.255"
#define BROADCAST_IP "192.168.0.255"



void error(const char* msg){
	perror(msg);
	exit(1);
}

void* echo_thread(void* arg){
	// pthread_detach(pthread_self());	// Alternativa
	int socket_c = *((int *)arg);
	char buff[BUFFER_SZ];
	// bzero(buff, BUFFER_SZ);
	int bytesRead;

	do{
		bzero(buff, BUFFER_SZ);

		if((bytesRead = read_socket(socket_c, buff, BUFFER_SZ)) < 0)
			error("Error reading from client\n");
		
		printf("Read from client %d: %s \n", socket_c, buff);

		if(write_socket(socket_c, buff, BUFFER_SZ) < 0)
			error("Error writing to client\n");
		
	}while(strncmp(buff, "end", 3) != 0);

	close(socket_c);
	pthread_exit(NULL);	
}

/*https://stackoverflow.com/questions/35206781/
timeout-on-an-accept-socket-function*/

int accept_timeout(int sockfd, struct sockaddr *cli_addr, socklen_t *addr_len){
	fd_set fdrs;
	FD_ZERO(&fdrs);
	FD_SET(sockfd, &fdrs);

	struct timeval tv;
	tv.tv_sec = ACCEPT_TIMEOUT;
	tv.tv_usec = 0;

	int n_fd = select(sockfd+1, &fdrs, NULL, NULL, &tv);

	if(n_fd == -1){
		error("Error select");
	}else if (n_fd == 0){
		return -2;
	}else if(FD_ISSET(sockfd, &fdrs)){
		int newsockfd = accept(sockfd, cli_addr, addr_len);
		if(newsockfd < 0)
			error("Error on accept");
		return newsockfd;
	}
	return -2;
}

int main(int argc, const char* argv[]){

	// ------------ Declaro variables ------------
	int sockfd;
	// int sockfd, newsockfd;

	struct sockaddr_in serv_addr, cli_addr, announce_addr;
	socklen_t addr_len = sizeof(serv_addr);
	// char buffer[BUFFER_SZ];

	// ------------ Configuro el socket UDP para anunciarme ------------
	int fd_announce = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(fd_announce < 0){
		error("Error opening socket\n");
	}
	// Lo pongo como brodcast
	/* Set socket to allow broadcast */
	int yes = 1;
	if(setsockopt(fd_announce, SOL_SOCKET, SO_BROADCAST, (void*) &yes, sizeof(yes)) < 0){
		perror("setsockopt() failed");
	}
	bzero((void*) &announce_addr, addr_len);
	announce_addr.sin_family = AF_INET;
	// serv_addr.sin_addr.s_addr 	= htonl(inet_addr(argv[2]));
	// if( inet_aton(BROADCAST_IP, &announce_addr.sin_addr) == 0){
	if( inet_aton(BROADCAST_IP, &announce_addr.sin_addr) == 0){
		error("Invalid Address");
	}
	announce_addr.sin_port = htons(PORT);
	char announce_msg[] = "Echo server on 127.0.0.1:6900"; 

	// ------------ Configuro el socket cliente ------------
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sockfd < 0){
		error("Error opening socket");
	}

	bzero((void*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family 		= AF_INET;
	// serv_addr.sin_addr.s_addr 	= htonl(INADDR_ANY);
	// if( inet_aton(SERVER_IP, &serv_addr.sin_addr) == 0){
	// 	error("Invalid Address");
	// }
	serv_addr.sin_addr.s_addr 	= htonl(INADDR_ANY);
	serv_addr.sin_port 			= htons(PORT);

	int val = 1;	// No tengo muy claro que pasa con el val
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	if(bind(sockfd, (struct sockaddr *) &serv_addr, addr_len) < 0){
		perror("Error on binding");
	}

	if(listen(sockfd, 5) < 0){
		perror("Error listening");
	}

	// ------------ Protocolo o lo que sea ------------
	// Configuro los atributos de los threads
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	// pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	// pthread_t threads[NUM_THREADS];
	int counter = 0;	// Voy a permitir 5 conexiones
	while(1){
		int newsockfd;

		do{
			// Falta anunciarme por UDP
			printf("Send announcement...\n");
			if(sendto(fd_announce, (void*) announce_msg, strlen(announce_msg)+1, 0, (struct sockaddr *) &announce_addr, sizeof(announce_addr)) == -1){
				perror("Error sending announcement");
			}
			// printf("Hola\n");
			newsockfd = accept_timeout(sockfd, (struct sockaddr *) &cli_addr, &addr_len);
		} while (newsockfd == -2);
		

		// if((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &addr_len)) < 0)
		// 	error("Error on accept");
		
		printf("Connected to %s:%d\n",
			inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
		
		pthread_t thr_id;
		if(pthread_create(&thr_id, &attr, echo_thread, (void*) &newsockfd) != 0){
			perror("pthread_create");
		}

		counter++;
	}

	
	pthread_attr_destroy(&attr);
	close(sockfd);

	pthread_exit(NULL);
	// return 0;
}