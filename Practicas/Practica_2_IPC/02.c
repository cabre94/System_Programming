#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

const int MSG_SIZE = 64;

void catch_int_info(int sig_num, siginfo_t *psiginfo, void *pcontext);
void catch_child(int sig_num, siginfo_t *psiginfo, void *pcontext);
void catch_child2(int sig_num);

void child(int* wPipe, int argc, char* argv[]);
void parent(int *rPipe);

int main(int argc, char* argv[]){

	int c2p[2];	// child -> parent 

	pid_t pid;
	printf("Mi pid es %d\n", getpid());

	// Seteo un handler para algunas señales
	struct sigaction act;

	

	if(pipe(c2p) == -1 ){
		perror("Error in creation pipe: ");
		return -1;
	}

	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = catch_child;
	sigfillset(&act.sa_mask);
	sigaction(SIGCHLD, &act, NULL);

	// Duplico el proceso
	pid = fork();

	// if(pid != 0){
	// 	// Child stopped or terminated
	// 	act.sa_flags = SA_SIGINFO;
	// 	act.sa_sigaction = catch_child;
	// 	sigfillset(&act.sa_mask);
	// 	sigaction(SIGCHLD, &act, NULL);

	// 	// // Broken pipe: write to pipe with no readers;
	// 	// act.sa_flags = SA_SIGINFO;
	// 	// act.sa_sigaction = catch_int_info;
	// 	// sigfillset(&act.sa_mask);
	// 	// sigaction(SIGPIPE, &act, NULL);

	// 	// signal(SIGCHLD, catch_child2);
	// }
	// else{
	// 	sleep(2);
	// }

	switch (pid){
		case 0:
			printf("Child (%d)\n", getpid());
			child(c2p, argc, argv);
			break;
		case -1:
			perror("Fork error: ");
			break;
		default:
			printf("Parent (%d) with child (%d)\n", getpid(), pid);
			parent(c2p);

			// int status;
			// pid_t pid = wait(&status);
			// assert(pid != -1);

			// printf("Mi hijo %d, retorno %d (0x%04x) %s\n", pid,
			// 	WEXITSTATUS(status), status,
			// 	WIFSIGNALED(status) ? "signaled" : "muerte natural"
			// );
			break;
	}

	return 0;
}


void catch_int_info(int sig_num, siginfo_t *psiginfo, void *pcontext){
	printf("Recibi la senal #%d %s de %d, code: %d\n",
		sig_num, strsignal(sig_num), psiginfo->si_pid, psiginfo->si_code);
}

void child(int* wPipe, int argc, char* argv[]){
	
	if(close(wPipe[0]) == -1){
		perror("Error close wPipe[0]");
		exit(EXIT_FAILURE);
	}

	if(dup2(wPipe[1], STDOUT_FILENO) == -1){
		perror("dup2 error: ");
		exit(EXIT_FAILURE);
	}

	//
	char *c_argv[argc + 1];
    memcpy(c_argv, argv, argc*sizeof(char*));
    c_argv[0] = "hello";
    c_argv[argc] = NULL;
	if (execv("hello", c_argv) < 0)
        perror("Child excecv:");
	// execl("hello", argv[0], argv[1], (char *) NULL);

	// No se si hace falta esto
	if(close(wPipe[1]) == -1){
		perror("Error close wPipe[1]");
		exit(EXIT_FAILURE);
	}

}

void parent(int *rPipe){

	if(close(rPipe[1]) == -1){
		perror("Error close rPipe[1]");
		exit(EXIT_FAILURE);
	}

	// Ahora leemos lo que nos haya mandado el child
	char buffer[MSG_SIZE];
	memset(buffer, 0, strlen(buffer));
	if( read(rPipe[0], buffer, MSG_SIZE) == -1){
		perror("Error reading from child");
		exit(EXIT_FAILURE);
	}
	// Por ultimo, mandamos standard output
	// if( write(STDOUT_FILENO, buffer, strlen(buffer)) == -1){
	// 	perror("Error writing to STDOUT_FILENO");
	// 	exit(EXIT_FAILURE);
	// }
	printf("Escribo %s\n", buffer);

	// Cierro los extremos del pipe
	if(close(rPipe[0]) == -1){
		perror("Error close rPipe[0]");
		exit(EXIT_FAILURE);
	}

	// getchar();


	
}

void catch_child(int sig_num, siginfo_t *psiginfo, void *pcontext){

	int status;
	printf("Se activo el handler\n");
	wait(&status); /*Mandamos null porque en siginfo_t ya esta todo*/



	printf("Recibi la senal #%d %s de %d, code: %d status: %d signaled: %d\n",
		sig_num, strsignal(sig_num), psiginfo->si_pid, psiginfo->si_code, WEXITSTATUS(status), WIFSIGNALED(status));
}

void catch_child2(int sig_num){
	int status;
	wait(&status);
	printf("status: %d signaled: %d\n", WEXITSTATUS(status), WIFSIGNALED(status));
}