# all : Socket.o Socket_Servidor.o Socket_Cliente.o server_fork client
all : server client

CPPFLAGS = -g -Wall -I.
CC = gcc

server : server.c
	${CC} ${CPPFLAGS} server.c -o server

client : client.c
	${CC} ${CPPFLAGS} client.c -o client

clean :
	rm server client
