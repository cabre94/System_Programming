# all : Socket.o Socket_Servidor.o Socket_Cliente.o server_fork client
all : tftp_packets.o utils.o server client

CPPFLAGS = -g -Wall -I.
CC = gcc

testing: tftp_packets.o test

test: test.c
	${CC} ${CPPFLAGS} tftp_packets.o test.c -o test

server : tftp_server.c
	${CC} ${CPPFLAGS} tftp_packets.o utils.o tftp_server.c -o tftp_server

client : tftp_client.c
	${CC} ${CPPFLAGS} tftp_packets.o utils.o tftp_client.c -o tftp_client


clean :
	rm *.o tftp_server tftp_client

