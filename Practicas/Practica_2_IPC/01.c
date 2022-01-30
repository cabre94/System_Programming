#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

const int MSG_SIZE = 64;

void catch_int_info(int sig_num, siginfo_t *psiginfo, void *pcontext);

void child(int *rPipe, int* wPipe);
void parent(int *rPipe, int* wPipe);

int main(){

	int p2c[2], c2p[2];	// parent -> child y viceversa

	pid_t pid;
	printf("Mi pid es %d\n", getpid());

	if( (pipe(p2c) == -1) || (pipe(c2p) == -1) ){
		perror("Error in creation pipe: ");
		exit(EXIT_FAILURE);
	}

	// Seteo un handler para algunas señales
	struct sigaction act, old;

	// Child stopped or terminated
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = catch_int_info;
	sigfillset(&act.sa_mask);
	sigaction(SIGCHLD, &act, &old);
	// Broken pipe: write to pipe with no readers;
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = catch_int_info;
	sigfillset(&act.sa_mask);
	sigaction(SIGPIPE, &act, &old);


	// Duplico el proceso
	pid = fork();

	switch (pid){
		case 0:
			printf("Child (%d)\n", getpid());
			child(p2c, c2p);
			break;
		case -1:
			perror("Fork error: ");
			break;
		default:
			sleep(3);
			printf("Parent (%d) with child (%d)\n", getpid(), pid);
			parent(c2p, p2c);
			break;
	}
}


void catch_int_info(int sig_num, siginfo_t *psiginfo, void *pcontext){
	printf("Recibi la senal #%d %s de %d, code: %d\n",
		sig_num,
		strsignal(sig_num),
		psiginfo->si_pid,
		psiginfo->si_code
	);
}

void child(int *rPipe, int* wPipe){
	
	// close(wPipe[0]); // extremo de lectura en pipe de escritura
	// close(rPipe[1]); // extremo de escritura en pipe de lectura
	if(close(wPipe[0]) == -1){
		perror("Error close wPipe[0]");
		exit(EXIT_FAILURE);
	}
	if(close(rPipe[1]) == -1){
		perror("Error close rPipe[1]");
		exit(EXIT_FAILURE);
	}

	// Tengo que leer del pipe, pasar a mayusculas y retornar
	char buffer[MSG_SIZE];
	memset(buffer, 0, MSG_SIZE);
	if( read(rPipe[0], buffer, MSG_SIZE) == -1){
		perror("Error reading from parent");
		exit(EXIT_FAILURE);
	}

	// Paso a mayusculas
	// for(char* c=buffer; (*c=toupper(*c)); ++c);
	for (int i = 0; i < strlen(buffer); i++){
		buffer[i] = toupper(buffer[i]);
	}

	// Mando al parent por el pipe
	if( write(wPipe[1], buffer, strlen(buffer)) == -1){
		perror("Error writing to parent");
		exit(EXIT_FAILURE);
	}

	// close(wPipe[1]);
	// close(rPipe[0]);
	if(close(wPipe[1]) == -1){
		perror("Error close wPipe[1]");
		exit(EXIT_FAILURE);
	}
	if(close(rPipe[0]) == -1){
		perror("Error close rPipe[0]");
		exit(EXIT_FAILURE);
	}
}

void parent(int *rPipe, int* wPipe){

	// close(wPipe[0]); // extremo de lectura en pipe de escritura
	// close(rPipe[1]); // extremo de escritura en pipe de lectura
	if(close(wPipe[0]) == -1){
		perror("Error close wPipe[1]");
		exit(EXIT_FAILURE);
	}
	if(close(rPipe[1]) == -1){
		perror("Error close rPipe[0]");
		exit(EXIT_FAILURE);
	}

	// Leo del standard input
	char buffer[MSG_SIZE];
	memset(buffer, 0, MSG_SIZE);
	if( read(STDIN_FILENO, buffer, MSG_SIZE) == -1){
		perror("Error reading from stdin");
		exit(EXIT_FAILURE);
	}

	// Mandamos el mensaje al child por el pipe
	if( write(wPipe[1], buffer, strlen(buffer)) == -1){
		perror("Error writing to child");
		exit(EXIT_FAILURE);
	}
	if(close(wPipe[1]) == -1){
		perror("Error close wPipe[1]");
		exit(EXIT_FAILURE);
	}

	// Ahora leemos lo que nos haya mandado el child
	memset(buffer, 0, strlen(buffer));
	if( read(rPipe[0], buffer, MSG_SIZE) == -1){
		perror("Error reading from child");
		exit(EXIT_FAILURE);
	}

	// Por ultimo, mandamos standard input
	if( write(STDOUT_FILENO, buffer, strlen(buffer)) == -1){
		perror("Error writing to child");
		exit(EXIT_FAILURE);
	}

	// close(wPipe[1]);
	// close(rPipe[0]);

	if(close(rPipe[0]) == -1){
		perror("Error close rPipe[0]");
		exit(EXIT_FAILURE);
	}
}