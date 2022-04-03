#ifndef _TFTP_PACKETS_H
#define _TFTP_PACKETS_H

typedef union packet{
	struct {
		int opcode;
		int block_n;
		char buff[512];
	} data;
	struct {
		int opcode;
		char filename[512];
	} rrq_wrq;
	struct {
		int opcode;
		int block_n;
	} ack;
	struct {
		int opcode;
		int code;
		char msg[512];
	} error;
} packet_t;

char *RRQ_create(char* filename);
char *WRQ_create(char* filename);
char *DATA_create(int block_n, char *data);
char *ACK_create(int block_n);
char *ERROR_create(int code, char* msg);

#endif