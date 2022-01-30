#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <string.h>


void catch_int(int sig_num){
	printf("Recibi la senal #%d %s\n", sig_num, strsignal(sig_num));
}

int main(){


	printf("Mi pid es %d\n", getpid());

	signal(SIGINT, catch_int);
	signal(SIGQUIT, catch_int);

	while (1){
		pause();
	}

	return 0;
}