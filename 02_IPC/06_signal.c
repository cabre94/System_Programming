#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

void catch_int_info(int sig_num, siginfo_t *psiginfo, void *pcontext){
	printf("Recibi la senal #%d %s de %d, code: %d\n",
		sig_num,
		strsignal(sig_num),
		psiginfo->si_pid,
		psiginfo->si_code
	);
}

void padre(pid_t pid){

	printf("Hello from padre (%d), hijo es %d\n", getpid(), pid);
	getchar();

	printf("Kill a %d\n", pid);
	kill(pid, SIGUSR1);
	pause();
}

void hijo(){
	printf("Hello from hijo, mi pid es %d, mi padre es %d\n", getpid(), getppid());
	pause();
}

int main(){

	pid_t pid;
	struct sigaction act;

	printf("Mi pid es %d\n", getpid());
	
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = catch_int_info;
	sigfillset(&act.sa_mask);

	sigaction(SIGUSR1, &act, NULL);

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