#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <assert.h>

int main()
{

	printf("Soy el padre mi pid es %d\n", getpid());

	pid_t pid = fork();

	int wstatus;

	// char *newargv[] = { "ls", "-la", NULL };
	// char *newenviron[] = { NULL };

	switch (pid)
	{
	case 0:
		printf("Soy el hijo %d\n", getpid());
		execlp("ls", "ls", "-la", NULL);
		assert(0);
		break;
	case -1:
		perror("Error en fork");
		break;
	default:
		printf("El proceso creado es %d\n", pid);
		pid = wait(&wstatus);
		printf("Murio el proceso %d\n", pid);
		printf("Murio con retorno %d\n", WEXITSTATUS(wstatus));
		getchar();
		break;
	}

	printf("Terminando proceso %d\n", getpid());

	return 32;
}