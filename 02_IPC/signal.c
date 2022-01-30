#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>


void sig_handler(int sig){
	printf("recibi la senal %d\n", sig);
	int status;
	pid_t pid =  wait(&status);
	printf("Murio el proceso %d\n", pid);
}


int main(){


	pid_t pid = fork();

	if(pid){
		signal(SIGCHLD, sig_handler);
		getchar();
	}else{
		sleep(2);
	}




	return 0;
}