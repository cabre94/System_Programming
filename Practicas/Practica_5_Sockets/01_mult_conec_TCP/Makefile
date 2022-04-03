# all : Socket.o Socket_Servidor.o Socket_Cliente.o server_fork client
all : Socket.o server_fork server_thread client

CPPFLAGS = -g -Wall -I.

server_fork : server_fork.c
	cc ${CPPFLAGS} Socket.o server_fork.c -o server_fork

server_thread : server_thread.c
	cc ${CPPFLAGS} Socket.o server_thread.c -o server_thread -lpthread

client : client.c
	cc ${CPPFLAGS} Socket.o client.c -o client

clean :
	rm *.o client server_fork server_thread
