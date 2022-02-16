# all : Socket.o Socket_Servidor.o Socket_Cliente.o server_fork client
all : Socket.o server_fork client

CPPFLAGS = -g -Wall -I.

server_fork : userver_fork.c
	cc ${CPPFLAGS} Socket.o userver_fork.c -o server_fork

client : uclient.c
	cc ${CPPFLAGS} Socket.o uclient.c -o client

clean :
	rm *.o client server_fork
