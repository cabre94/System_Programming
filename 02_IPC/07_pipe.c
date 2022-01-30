#include <unistd.h>
#include <sys/types.h>
// #include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>


const char *msg = "abcdefghijklmnopqrstuvwxyz\n";
int pfd[2];

void catch_child(int sig_num, siginfo_t *psiginfo, void *pcontext);
void padre(pid_t pid);
void hijo();

int main(int argc, const char** argv){

	// int pfd[2];

	// const char *msg = "abcdefghijklmnopqrstuvwxyz\n";

	pid_t pid;
	struct sigaction act;

	printf("Mi pid es %d\n", getpid());
	
	
	act.sa_flags 		= SA_SIGINFO;
	act.sa_sigaction 	= catch_child;
	sigfillset(&act.sa_mask);

	sigaction(SIGCHLD, &act, NULL);


	if(pipe(pfd) == -1){
		perror("Error en pipe");
		exit(EXIT_FAILURE);
	}

	pid = fork();

	switch (pid){
		case 0:
			hijo();
			break;
		case -1:
			perror("fork error");
			break;	
		default:
			padre(pid);
			break;
	}




	return 0;
}



void catch_child(int sig_num, siginfo_t *psiginfo, void *pcontext){

	wait(NULL); /*Mandamos null porque en siginfo_t ya esta todo*/

	printf("Recibi la senal #%d %s de %d, code: %d\n",
		sig_num,
		strsignal(sig_num),
		psiginfo->si_pid,
		psiginfo->si_code
	);
}

void padre(pid_t pid){
	
	int ret;

	close(pfd[0]); // Cierro el extremo de Lectura

	getchar();

	ret = write(pfd[1], msg, strlen(msg));
	if(ret == -1){
		perror("Write en el pipe");
		exit(EXIT_FAILURE);
	}

	printf("(%d) pipe closes, bye bye.\n", getpid());
	close(pfd[1]);

	while (1){
		pause();
	}
	
}

void hijo(){

	char c;

	printf("Hello from hijo, mi pid es %d, mi padre es %d\n", getpid(), getppid());

	close(pfd[1]); // Cierro el extremo de escritura

	// int c_read;

	// Leo de a un caracter, habria que chequear que no tira error...
	while( read(pfd[0], &c, 1)){
		putchar(c);
	}

	printf("(%d) pipe closes, bye bye.\n", getpid());
	close(pfd[0]);
}