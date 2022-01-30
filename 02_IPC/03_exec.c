#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>




int main(int argc, char* argv[]){

	pid_t pid;
	int status;

	printf("[%s] mi pid es %d\n", argv[0], getpid());

	pid = fork();

	switch (pid){
		case 0:
			printf("Hello from hijo, mi pid es %d, mi padre es %d\n", getpid(), getppid());
			execl("/usr/bin/ls", "ls", "-l", "/", (char*) NULL);
			perror("error en exec: ");
			assert(0);
		case -1:
			perror("fork error");
			break;	
		default:
			printf("Hello from padre (%d), hijo es %d\n", getpid(), pid);

			puts("paktc");
			getchar();

			pid = wait(&status);
			assert(pid != -1);

			printf("Mi hijo %d, retorno %d (0x%04x) %s\n", pid,
				WEXITSTATUS(status), status,
				WIFSIGNALED(status) ? "signaled" : "muerte natural"
			);

			puts("paktc");
			getchar();

			break;
	}


	return 0;
}