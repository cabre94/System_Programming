#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <assert.h>
#include <netdb.h>

#include "tftp_packets.h"
#include "utils.h"

int main(int argc, char* argv[]){

	// =============== Declaracion de variables ===============
	int socket_fd, bytesRead;
	char *file = argv[3]; // file name on which operation has to be done

	struct sockaddr_in serv_addr;
	socklen_t addr_len = sizeof(serv_addr);
	char buffer[BUFFER_SZ];
	char data[MAXSIZE_BLOCK];
	char s[INET_ADDRSTRLEN];

	if(argc != 4){// CHECKS IF args ARE VALID
		fprintf(stderr,"Usage: %s SEND/RECV server filename\n", argv[1]);
		exit(1);
	}
	// =============== Configuracion cliente ===============
	if( (socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
		die("socket");
	}

	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family 		= AF_INET;
	serv_addr.sin_addr.s_addr 	= inet_addr(argv[2]);
	serv_addr.sin_port 			= htons(PORT);  // Any local port will do

	//
	if(strcmp(argv[1], "RECV") == 0){
		printf("Unimplmented\n");
		assert(0);
	}else if(strcmp(argv[1], "SEND") == 0){
		// ============== Mando un WRQ
		char* wrq = WRQ_create(file);
		char last_send_msg[BUFFER_SZ];
		strncpy(last_send_msg, wrq, strlen(wrq));
		
		if(sendto(socket_fd, wrq, strlen(wrq)+1, 0, (struct sockaddr *) &serv_addr, addr_len) == -1){
			die("sendto()");
		}
		printf("WRQ sent for %s\n", file);
		free(wrq);

		// ============== Espero el ACK0
		for(int t = 0; t <= MAX_TRIES; t++){
			if(t == MAX_TRIES){
				printf("Client: max number of tries waiting ACK0\n");
				exit(1);
			}

			bytesRead = recvFromTimeout(socket_fd, buffer, ACK_SZ, (struct sockaddr *) &serv_addr, &addr_len, TIME_OUT);
			if(bytesRead == -2){	// timeout
				printf("Try %d waiting for ACK0\n", t+1);
				if(sendto(socket_fd, last_send_msg, strlen(last_send_msg)+1, 0, (struct sockaddr *) &serv_addr, addr_len) == -1){
					die("sendto()");
				}
				printf("WRQ sent for %s again\n", file);
			}else if(bytesRead == -1)
				die("recvFromTimeout:");
			else
				break;

		}

		printf("CLIENT: got packet from %s\n", inet_ntop(serv_addr.sin_family, &serv_addr.sin_addr, s, sizeof(s)));
		printf("CLIENT: packet is %d bytes long\n", bytesRead);

		if(buffer[0]=='0' && buffer[1]=='4'){	// Es un ACK
		// if(1){
			// Abro el file
			int fd = open(file, O_RDONLY);
			if(fd < 0){
				die("Error opening file: ");
			}
			printf("File: %s open\n", file);

			// int bytesSend;
			int block_n = 1;
			char* data_packet;
			int bytesRecv;
			// sleep(2);

			do{
				// Leo 512 bytes
				bytesRead = read(fd, data, MAXSIZE_BLOCK);

				// Creo el DATA packet
				printf("Client send %d bytes\n", bytesRead);
				data_packet = DATA_create(block_n, data);
				if(sendto(socket_fd, data_packet, bytesRead+4, 0, (struct sockaddr *) &serv_addr, addr_len) == -1){
					die("sendto()");
				}
				printf("Client send %d bytes\n", bytesRead);
				strncpy(last_send_msg, data_packet, bytesRead+4);


				// Espero el ACK
				printf("Espero ACK %d\n", block_n);
				for(int t = 0; t <= MAX_TRIES; t++){
					if(t == MAX_TRIES){
						printf("Client: max number of tries waiting ACK0\n");
						exit(1);
					}

					bytesRecv = recvFromTimeout(socket_fd, buffer, ACK_SZ, (struct sockaddr *) &serv_addr, &addr_len, TIME_OUT);

					if(bytesRecv == -2){	// timeout
						printf("Try %d waiting for ACK0\n", t+1);
						if(sendto(socket_fd, last_send_msg, bytesRead+4, 0, (struct sockaddr *) &serv_addr, addr_len) == -1){
							die("sendto()");
						}
						printf("Data sent for %d again\n", bytesRead);
					}else if(bytesRecv == -1)
						die("recvFromTimeout:");
					else
						break;

				}

				printf("CLIENT: got packet from %s\n", inet_ntop(serv_addr.sin_family, &serv_addr.sin_addr, s, sizeof s));
				printf("CLIENT: packet is %d bytes long\n", bytesRecv);

				if(buffer[0] == '0' && buffer[1] == '5'){
					printf("Error recibiendo el ACK\n");
					assert(0);
				}

				free(data_packet);
				data_packet = NULL;

				block_n++;
				if(block_n>99)
					block_n = 1;

			} while (bytesRead == MAXSIZE_BLOCK);

			printf("TERMINE\n");
			close(fd);
			

		}else{
			fprintf(stderr,"CLIENT ACK: expecting but got: %s\n", buffer);
			exit(1);
		}

	}else{
		// chequear si hay que liberar algo
	}

	close(socket_fd);

	return 0;
}