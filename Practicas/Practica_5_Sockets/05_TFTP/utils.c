#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "utils.h"

void die(char *s){
	perror(s);
	exit(1);
}

int read_from(int sockfd, char *buffer, int length, struct sockaddr *addr, socklen_t *addr_len){
	int bytesRead;
	int totalRead = 0;

	while(totalRead < length){
		printf("%d\n",totalRead);
		// bytesRead = read(sockfd, buffer+totalRead, length-totalRead);
		bytesRead = recvfrom(sockfd, buffer+totalRead, length-totalRead, 0, addr, addr_len);
		if (bytesRead > 0){
			totalRead += bytesRead;
		}else if(bytesRead == 0){
			return totalRead;
		}else if(errno != EINTR){
			return -1;
		}
	}

	return totalRead;
}

int recvFromTimeout(int sock, char *buf, int len, struct sockaddr *addr, socklen_t *addr_len, int timeoutSecs){
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

// int send_to(int sockfd, const char* buff, int len, const struct sockaddr *addr, const socklen_t addr_len){
// 	int bytesSent;
// 	int totalSent = 0;

// 	while (totalSent < len){
// 		bytesSent = sendto(sockfd, buff+totalSent, len-totalSent, 0, addr, addr_len);
// 		if(bytesSent > 0)
// 			totalSent += bytesSent;
// 		else if(bytesSent == 0 || (bytesSent == -1 && errno != EINTR))
// 			return -1;
// 	}
	
// 	return totalSent;
// }
