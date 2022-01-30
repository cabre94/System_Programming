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

const int MSG_SIZE = 32;

void catch_child(int sig_num, siginfo_t *psiginfo, void *pcontext){

	int status;
	printf("Se activo el handler\n");
	
	wait(&status); /*Mandamos null porque en siginfo_t ya esta todo*/

	printf("Recibi la senal #%d %s de %d, code: %d status: %d signaled: %d señal: %d\n",
		sig_num, strsignal(sig_num), psiginfo->si_pid, psiginfo->si_code, WEXITSTATUS(status), WIFSIGNALED(status), WTERMSIG(status));
}

int main(int argc, char *argv[]){

	int pid;
	struct sigaction act;

	int c2p[2];	// child -> parent 



	// Child stopped or terminated
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = catch_child;
	sigfillset(&act.sa_mask);
	sigaction(SIGCHLD, &act, NULL);

	if(pipe(c2p) == -1 ){
		perror("Error in creation pipe: ");
		return -1;
	}

	pid = fork();



	switch (pid){
		case 0:{
			// sleep(5);
			printf("Child (%d)\n", getpid());
			assert(close(c2p[0]) != -1);
			assert(dup2(c2p[1], STDOUT_FILENO) != -1);
			// execl("hello", "hello", argv[1], (char *) NULL);

			char *c_argv[argc + 1];
			memcpy(c_argv, argv, argc*sizeof(char*));
			c_argv[0] = "hello";
			c_argv[argc] = NULL;

			sleep(5);
			if (execv("hello", c_argv) < 0)
				perror("Child excecv:");
			
			*((int*) NULL) = 0;
			// child(c2p, argc/, argv);
			break;
		}
		case -1:
			perror("Fork error: ");
			break;
		default:{
			printf("Parent (%d) with child (%d)\n", getpid(), pid);
			char buffer[MSG_SIZE];
			assert(close(c2p[1]) != -1);
			assert(read(c2p[0], buffer, MSG_SIZE) != -1);
			// assert(write(STDOUT_FILENO, buffer, strlen(buffer)) != -1);
			printf("Escribo %s\n", buffer);
			assert(close(c2p[0]) != -1);
			// parent(c2p);
			// sleep(5);
			// printf("Antes del getchar\n");
			int x =	getchar();
			printf("%d\n", x);
			printf("%s\n", strerror(errno));

			// wait(NULL);
			break;
		}
	}


	return 0;

}