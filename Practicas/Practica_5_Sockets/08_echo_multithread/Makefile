# all : Socket.o Socket_Servidor.o Socket_Cliente.o server_fork client
all : Socket.o server client listener

CPPFLAGS = -g -Wall -I.

# server_fork : server_fork.c
# 	cc ${CPPFLAGS} Socket.o server_fork.c -o server_fork

server : server.c
	gcc ${CPPFLAGS} Socket.o server.c -o server -lpthread

client : client.c
	gcc ${CPPFLAGS} Socket.o client.c -o client

listener : listener.c
	gcc ${CPPFLAGS} listener.c -o listener

clean :
	rm *.o client server listener
