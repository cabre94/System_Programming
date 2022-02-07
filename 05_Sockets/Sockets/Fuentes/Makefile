all : Socket.o Socket_Servidor.o Socket_Cliente.o Servidor Cliente

CPPFLAGS = -g -Wall -I.

Servidor : Servidor.c
	cc -g -Wall -I. Socket.o Socket_Servidor.o Servidor.c -o Servidor

Cliente : Cliente.c
	cc -g -Wall -I. Socket.o Socket_Cliente.o Cliente.c -o Cliente

clean :
	rm *.o Cliente Servidor
