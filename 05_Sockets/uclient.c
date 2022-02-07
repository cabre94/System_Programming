#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BUFFER_SZ 256

void error(const char* msg){
	perror(msg);
	exit(2);
}


int main(int argc, char *argv[]){

	int sockfd, n;

	struct sockaddr_un serv_addr;
	char buffer[BUFFER_SZ];

	if(argc != 2){
		fprintf(stderr, "Usage %s sockName\n", argv[0]);
		exit(1);
	}

	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

	if(sockfd < 0){
		error("Error opening socket\n");
	}

	bzero((void*) &serv_addr, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, argv[1]);

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