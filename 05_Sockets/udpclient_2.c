#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_SZ 512
#define NPACK 10
#define PORT "9930"

void die(char *s){
	perror(s);
	exit(1);
}

int main(int argc, char *argv[]){

	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	
	int s, i, rv;

	struct sockaddr_in serv_addr;
	struct sockaddr_storage their_addr;
	// struct hostent *server;
	socklen_t slen = sizeof(serv_addr);
	socklen_t addr_len;
	char buffer[BUFFER_SZ];

	// ----------------
	char *server = argv[1];// server address

	memset(&hints, 0, sizeof(hints));
	hints.ai_family 	= AF_INET;
	hints.ai_socktype 	= SOCK_DGRAM;
	hints.ai_flags 		= 0;
	hints.ai_protocol	= IPPROTO_UDP;

	if((rv = getaddrinfo(server, PORT, &hints, &servinfo)) != 0){
		fprintf(stderr, "CLIENT: getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockfd == -1)
			continue;

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) != -1)
			break;                  /* Success */

		close(sockfd);
	}
	if(p == NULL){
		fprintf(stderr, "CLIENT: failed to bind socket\n");
		return 2;
	}



	// if( (s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
	// 	die("socket");
	// }

	// server = gethostbyname(argv[1]);
	// if(server == NULL){
	// 	fprintf(stderr, "Error, no such host\n");
	// 	exit(0);
	// }

	// bzero((char*) &serv_addr, sizeof(serv_addr));
	// serv_addr.sin_family = AF_INET;
	// bcopy(
	// 	(char*) server->h_addr,
	// 	(char*) &serv_addr.sin_addr.s_addr,
	// 	server->h_length
	// );
	// serv_addr.sin_port = htons(PORT);

	for(i = 0; i < NPACK; ++i){
		printf("Sending packet %d\n", i);
		sprintf(buffer, "This is packet %d\n", i);

		if(sendto(sockfd, buffer, BUFFER_SZ, 0, p->ai_addr, p->ai_addrlen) == -1){
		// if(sendto(sockfd, buffer, BUFFER_SZ, 0, (struct sockaddr *) &serv_addr, slen) == -1){
			die("sendto()");
		}
	}

	close(sockfd);

	return 0;
}