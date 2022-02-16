#ifndef _SOCKET_H
#define _SOCKET_H

int read_socket(int sockfd, char *buffer, int length);
int write_socket(int sockfd, char *buffer, int length);

#endif