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

#include "tftp_packets.h"
#include "utils.h"



int main(int argc, char *argv[]){

	// =============== Declaracion de variables ===============
	int socket_fd, bytesRead;
	struct sockaddr_in si_me, si_other;
	socklen_t addr_len = sizeof(si_other);
	char buffer[BUFFER_SZ];

	char s[INET_ADDRSTRLEN];


	// =============== Configuracion servidor ===============
	bzero((char*) &si_me, sizeof(si_me));
	si_me.sin_family 		= AF_INET;
	si_me.sin_port 			= htons(PORT);			// host to network short
	si_me.sin_addr.s_addr 	= htonl(INADDR_ANY);	// host to network long

	if((socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		die("socket");
	
	if(bind(socket_fd, (struct sockaddr *) &si_me, sizeof(si_me)) == -1){
		close(socket_fd);
		die("bind");
	}


	// =============== Ya esta? aca lo uso? ===============
	// Espero por un WRQ o un RRQ.
	printf("Waiting request...");
	if( (bytesRead = recvfrom(socket_fd, buffer, BUFFER_SZ, 0, (struct sockaddr *) &si_other, &addr_len)) == -1){
	// if( (bytesRead = read_from(socket_fd, buffer, BUFFER_SZ, (struct sockaddr *) &si_other, &addr_len)) == -1){
		die("recvFrom error");
	}
	printf("SERVER: got packet from %s. ", inet_ntop(si_other.sin_family, &si_other.sin_addr, s, sizeof(s)));
	printf("Packet is %d bytes long\n", bytesRead);

	if(buffer[0] == '0' && buffer[1] == '1'){ // RRQ
		printf("Unimplmented\n");
		assert(0);
	}else if(buffer[0] == '0' && buffer[1] == '2'){	// WRQ
		// Saco la info del file
		char filename[MAXSIZE_FILENAME];
		strcpy(filename, "server-");
		strcat(filename, buffer+2);

		// Creo el archivo
		int fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0666);
		// int fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
		if(fd < 0){
			die("Error creating file: ");
			// Aca habria que hacer muchas mas cosas
		}
		printf("File: %s created\n", filename);


		// ================= Mando el ACK =================
		char *ack = ACK_create(0);
		char last_recv_message[BUFFER_SZ];
		strncpy(last_recv_message, buffer, BUFFER_SZ);
		char last_ack[ACK_SZ];
		strncpy(last_ack, ack, ACK_SZ);

		if(sendto(socket_fd, ack, ACK_SZ, 0, (struct sockaddr *) &si_other, addr_len) == -1){
			die("sendto() ACK");
		}
		printf("ACK0 sent\n");
		free(ack);

		// 
		// int ack_num = 1;
		int ack_num;
		int bytesWritten;
		do{
			// Espero un paquete
			// printf("Espero packet %d\n", ack_num);
			bytesRead = recvFromTimeout(socket_fd, buffer, BUFFER_SZ, (struct sockaddr *) &si_other, &addr_len, TIME_OUT);
			if(bytesRead == -2)
				die("Timeout: ");
			else if(bytesRead == -1)
				die("recvFromTimeout:");
			
			printf("SERVER: got packet from %s\n", inet_ntop(si_other.sin_family, &si_other.sin_addr, s, sizeof(s)));
			printf("SERVER: packet is %d bytes long\n", bytesRead);

			// Chequeo si es lo mismo que se mando antes
			if(strncmp(buffer, last_recv_message, BUFFER_SZ) == 0){
				if(sendto(socket_fd, last_ack, ACK_SZ, 0, (struct sockaddr *) &si_other, addr_len) == -1){
					die("sendto() ACK");
				}
				continue;
			}

			// Lo escribo en el fd la data
			bytesWritten = write(fd, buffer+4, bytesRead-4);
			strncpy(last_recv_message, buffer, bytesRead-4);

			// Mando el ACK
			ack_num = (buffer[2]-'0')*10 + (buffer[3]-'0') ;
			printf("Written %d bytes - Try to send ACK%d\n", bytesWritten, ack_num);
			ack = ACK_create(ack_num);
			if(sendto(socket_fd, ack, ACK_SZ, 0, (struct sockaddr *) &si_other, addr_len) == -1){
				die("sendto() ACK");
			}
			printf("ACK%d sent\n", ack_num);
			strcpy(last_ack, ack);

			free(ack);
		}while(bytesWritten == MAXSIZE_BLOCK);

		printf("File transfered successfully\n");
		close(fd);
		
	}else{
		// Ni me acuerdo que tengo que liberar
	}


	close(socket_fd);

	return 0;
}