# all : Socket.o Socket_Servidor.o Socket_Cliente.o server_fork client
all : Queue.o UDTs.o main

CPPFLAGS = -g -Wall -I.

# server_fork : server_fork.c
# 	cc ${CPPFLAGS} Socket.o server_fork.c -o server_fork

main : main.c
	gcc ${CPPFLAGS} Queue.o UDTs.o main.c -o exe -lpthread

clean :
	rm *.o exe
