#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFFER_SZ 512
#define NPACK 10
#define PORT 9930

void die(char *s){
	perror(s);
	exit(1);
}

int main(){

	struct sockaddr_in si_me, si_other;
	int s, i;

	socklen_t slen = sizeof(si_other);
	char buffer[BUFFER_SZ];

	if( (s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1 ){
		die("socket");
	}

	bzero((char*) &si_me, sizeof(si_me));

	si_me.sin_family 		= AF_INET;
	si_me.sin_port 			= htons(PORT);			// host to network short
	si_me.sin_addr.s_addr 	= htonl(INADDR_ANY);	// host to network long
	printf("PORT: %d (0x%x). PORT en network order: %d (0x%x)\n",
		PORT, PORT, si_me.sin_port, si_me.sin_port);

	if(bind(s, (struct sockaddr *) &si_me, sizeof(si_me)) == -1){
		die("bind");
	}

	//
	for(i = 0; i < NPACK; ++i){
		if(recvfrom(s, buffer, BUFFER_SZ, 0, (struct sockaddr *) &si_other, &slen) == -1){
			die("recvfrom()");
		}
		printf("Received packet from %s:%d\nData: %s\n",
			inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buffer);
	}

	close(s);

	return 0;
}