#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>


#define BUFFER_SZ 256

void error(const char* msg){
	perror(msg);
	exit(2);
}


int main(int argc, char *argv[]){

	int sockfd, portno, n;

	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[BUFFER_SZ];

	if(argc < 3){
		fprintf(stderr, "Usage %s sockName\n", argv[0]);
		exit(1);
	}

	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 0){
		error("Error opening socket\n");
	}
	
	server = gethostbyname(argv[1]);
	if(server == NULL){
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);	
	}


	bzero((void*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy(
		(char*) server->h_addr,
		(char*) &serv_addr.sin_addr.s_addr,
		server->h_length
	);
	serv_addr.sin_port = htons(portno);

	if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		error("Error on connect");
	}

	printf("Please enter the messege: ");
	bzero(buffer, BUFFER_SZ);

	fgets(buffer, BUFFER_SZ, stdin);

	n = write(sockfd, buffer, BUFFER_SZ);
	if(n < 0){
		error("Error writing to socket");
	}
	n = read(sockfd, buffer, BUFFER_SZ);
	if(n < 0){
		error("Error reading from socket");
	}

	printf("%s\n", buffer);

	close(sockfd);
	
	return 0;
}