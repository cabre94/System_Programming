#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <assert.h>

void catch_child(int sig_num, siginfo_t *psiginfo, void *pcontext){

	// wait(NULL); /*Mandamos null porque en siginfo_t ya esta todo*/

	printf("Recibi la senal #%d %s de %d, code: %d\n",
		sig_num, strsignal(sig_num), psiginfo->si_pid, psiginfo->si_code);
}

int main(int argc, char* argv[]){

	int wstatus;
	pid_t pid;

	printf("[%s] mi pid es %d\n", argv[0], getpid());

	struct sigaction act;

	// Child stopped or terminated
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = catch_child;
	sigfillset(&act.sa_mask);
	sigaction(SIGCHLD, &act, NULL);

	pid = fork();

	switch (pid){
		case 0:
			printf("Hello from hijo, mi pid es %d, mi padre es %d\n", getpid(), getppid());
			sleep(5);
			printf("Bye from hijo %d\n", getpid());
			return 42;
		case -1:
			perror("fork error");
			break;	
		default:
			printf("Hello from padre (%d), hijo es %d\n", getpid(), pid);

			puts("paktc");
			getchar();

			pid = wait(&wstatus);
			assert(pid != -1);

			printf("Mi hijo %d, retorno %d (0x%04x) %s\n", pid,
				WEXITSTATUS(wstatus), wstatus,
				WIFSIGNALED(wstatus) ? "signaled" : "muerte natural"
			);

			puts("paktc");
			getchar();

			break;
	}

	return 0;
}