#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

typedef struct _RRW{
	char* filename;
};

int read_socket(int sockfd, char *buffer, int length){
	int bytesRead;
	int totalRead = 0;

	while(totalRead < length){
		bytesRead = read(sockfd, buffer+totalRead, length-totalRead);
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

int recvtimeout(int sock, char *buf, int len, int timeoutSecs){
	fd_set fdrs;
	FD_ZERO(&fdrs);
	FD_SET(sock, &fdrs);

	struct timeval tv;
	tv.tv_sec = timeoutSecs;
	tv.tv_usec = 0;

	int n_fd = selec(sock+1, &fdrs, NULL, NULL, &tv);

	if(n_fd == -1){
		perror("Error select");
		return -1;
	}else if (n_fd == 0){
		return -2;
	}else if(FD_ISSET(sock, &fdrs)){
		return read_socket(sock, buf, len);
	}
	return -2;
}