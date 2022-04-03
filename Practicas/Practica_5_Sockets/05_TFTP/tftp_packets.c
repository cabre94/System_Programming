#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tftp_packets.h"
#include "utils.h"

// Opcode - filename - 0 - mode - 0
char* RRQ_create(char* filename){
	char* rrq = (char*) calloc(3+strlen(filename), sizeof(char));
	strcat(rrq, "01");
	strcat(rrq, filename);
	return rrq;
}

char* WRQ_create(char* filename){
	char* wrq = (char*) calloc(3+strlen(filename), sizeof(char));
	strcat(wrq, "02");
	strcat(wrq, filename);
	return wrq;
}

// DATA packet
// Opcode (2 bytes) - block_number (2 bytes) - Data (0-512 bytes)
char *DATA_create(int block_n, char *data){
	// char* packet = (char*) calloc(1024, sizeof(char));
	char* packet = (char*) calloc(BUFFER_SZ, sizeof(char));
	strcat(packet, "03");
	
	block_n = block_n % 100;	// No se que onda si nos pasamos
	packet[2] = (block_n / 10) + '0';
	packet[3] = (block_n % 10) + '0';
	// packet[4] = '\0';

	// strncat(packet, data, strlen(data)); // Esto no pone un \0
	strncat(packet, data, 512); // Esto no pone un \0
	return packet;
}

// opcode (2 bytes) - block_number (2 bytes)
char* ACK_create(int block_n){
	char* ack = (char *) calloc(4, sizeof(char));
	strcat(ack, "04");

	block_n = block_n % 100;	// No se que onda si nos pasamos
	ack[2] = (block_n / 10) + '0';
	ack[3] = (block_n % 10) + '0';

	return ack;
}

// opcode (2 bytes) - Error_code (2 bytes) - msg - 0
char* ERROR_create(int code, char* msg){
	char* packet = (char*) calloc(4+strlen(msg), sizeof(char));
	strcat(packet, "05");

	code = code % 100;	// No se que onda si nos pasamos
	packet[2] = (code / 10) + '0';
	packet[3] = (code % 10) + '0';

	strcat(packet, msg); // Esto no pone un \0
	return packet;
}