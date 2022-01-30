#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main()
{

	printf("Soy el padre mi pid es %d\n", getpid());

	pid_t pid = fork();

	int wstatus;

	switch (pid)
	{
	case 0:
		printf("Soy el hijo %d\n", getpid());
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