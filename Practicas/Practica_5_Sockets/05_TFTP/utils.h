#ifndef _TFTP_UTILS_H
#define _TFTP_UTILS_H

#include <sys/socket.h>

#define BUFFER_SZ 516	// 512 (data) + 4 (opcode+#block) + 1(un 0) o sin el cero?
#define PORT 6900
#define TIME_OUT 5
#define MAXSIZE_FILENAME 64
#define MAXSIZE_BLOCK 512
#define ACK_SZ 4
#define MAX_TRIES 3 

void die(char *s);

int read_from(int sockfd, char *buffer, int length, struct sockaddr *addr, socklen_t *addr_len);

int recvFromTimeout(int sock, char *buf, int len, struct sockaddr *addr, socklen_t *addr_len, int timeoutSecs);

int send_to(int sockfd, const char* buff, int len, const struct sockaddr *addr, const socklen_t addr_len);


#endif