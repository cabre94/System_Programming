#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include "Socket.h"


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

int write_socket(int sockfd, char *buffer, int length){
	int bytesWritten;
	int totalWritten = 0;

	while (totalWritten < length){
		bytesWritten = write(sockfd, buffer+totalWritten, length-totalWritten);
		if(bytesWritten > 0){
			totalWritten += bytesWritten;
		}else if(bytesWritten == 0){
			return totalWritten;
		}else if(errno != EINTR){
			return -1;
		}
	}

	return totalWritten;
}
