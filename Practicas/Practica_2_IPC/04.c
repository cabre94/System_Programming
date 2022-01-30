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

const int BUFFER_SZ = 16;

void catch_int_info(int sig_num, siginfo_t *psiginfo, void *pcontext);
void catch_child(int sig_num, siginfo_t *psiginfo, void *pcontext);

void child(int *rPipe, int* wPipe);
void parent(int *rPipe, int* wPipe);

int main(){

	int p2c[2], c2p[2];	// parent -> child y viceversa

	pid_t pid;
	printf("Mi pid es %d\n", getpid());

	

	// Seteo un handler para algunas señales
	struct sigaction act;

	// Child stopped or terminated
	// act.sa_flags = SA_SIGINFO;
	// act.sa_sigaction = catch_child;
	// sigfillset(&act.sa_mask);
	// sigaction(SIGCHLD, &act, NULL);
	
	// // Broken pipe: write to pipe with no readers;
	// act.sa_flags = SA_SIGINFO;
	// act.sa_sigaction = catch_int_info;
	// sigfillset(&act.sa_mask);
	// sigaction(SIGPIPE, &act, NULL);


	while(1){
		if( (pipe(p2c) == -1) || (pipe(c2p) == -1) ){
			perror("Error in creation pipe: ");
			exit(EXIT_FAILURE);
		}

		pid = fork();	// Duplico el proceso

		switch (pid){
			case 0:
				printf("Child (%d)\n", getpid());
				child(p2c, c2p);
				break;
			case -1:
				perror("Fork error: ");
				break;
			default:
				printf("Parent (%d) with child (%d)\n", getpid(), pid);
				parent(c2p, p2c);
				// int status;
				// wait(&status);
				fseek(stdin, 0, SEEK_END);
				break;
		}
		if(!pid){
			break;
		}



	}

	return 0;

}



void child(int *rPipe, int* wPipe){
	
	if(close(wPipe[0]) == -1){
		perror("Error close wPipe[0]");
		exit(EXIT_FAILURE);
	}
	if(close(rPipe[1]) == -1){
		perror("Error close rPipe[1]");
		exit(EXIT_FAILURE);
	}

	if(dup2(rPipe[0], STDIN_FILENO) == -1){
		perror("dup2 error: ");
		exit(EXIT_FAILURE);
	}
	if(dup2(wPipe[1], STDOUT_FILENO) == -1){
		perror("dup2 error: ");
		exit(EXIT_FAILURE);
	}


	if(close(wPipe[1]) == -1){
		perror("Error close wPipe[1]");
		exit(EXIT_FAILURE);
	}
	if(close(rPipe[0]) == -1){
		perror("Error close rPipe[0]");
		exit(EXIT_FAILURE);
	}
	
	// if(execl("/usr/bin/perl", "perl", "-pe", "$_ = uc $_", (char *) NULL) < 0){
	if(execl("filtro", "filtro", (char *) NULL) < 0){
		perror("Child execl");
		exit(EXIT_FAILURE);
	}
}

void parent(int *rPipe, int* wPipe){

	if(close(wPipe[0]) == -1){
		perror("Error close wPipe[0]");
		exit(EXIT_FAILURE);
	}
	if(close(rPipe[1]) == -1){
		perror("Error close rPipe[1]");
		exit(EXIT_FAILURE);
	}

	fd_set rfds;
	struct timeval tv;
	
	int rd1, rd2, n_fd;
	char send[BUFFER_SZ];
	char receive[BUFFER_SZ];
	char no_data[] = "No data within five seconds\n";

	int flag_stdin = 1;
	int flag_pipe = 1;

	while(1){
		FD_ZERO(&rfds);
		if(flag_stdin){
			FD_SET(STDIN_FILENO, &rfds);
		}
		FD_SET(rPipe[0], &rfds);

		tv.tv_sec = 5;
		tv.tv_usec = 0;

		n_fd = select(rPipe[0]+1, &rfds, NULL, NULL, &tv);


		switch (n_fd){
			case -1:
				perror("Father select: ");
				break;
			case 0:
				write(1, no_data, sizeof(no_data));
				break;
			default:
				if(FD_ISSET(STDIN_FILENO, &rfds)){
					if( (rd1 = read(STDIN_FILENO, send, BUFFER_SZ)) == -1){
						perror("Father read from stdin");
						exit(EXIT_FAILURE);
					}
					if(rd1 > 0){
						if(write(wPipe[1], send, rd1) == -1){
							perror("Parent write to child");
							exit(EXIT_FAILURE);
						}
						if(close(wPipe[1]) == -1){
							perror("Parent close wPipe[1]:");
							exit(EXIT_FAILURE);
						}
						flag_stdin = 0;
					}
					break;
				}
				if (FD_ISSET(rPipe[0], &rfds))
				{
					/* leo del pipe child->father */
					if ((rd2 = read(rPipe[0], receive, sizeof(receive))) < 0)
						perror("Father read from child");
					if (rd2 > 0)
					{
						/*cierro el pipe child-father*/
						if (close(rPipe[0]) < 0)
							perror("Father close child_to_father[0]:");

						/* escribo a stdout */
						if (write(STDOUT_FILENO, receive, rd2) < 0)
							perror("Father write to stdout");

						flag_pipe = 0;
					}
				}
        }
        if (!flag_pipe)
        {
            break;
        }

	}

}


void catch_int_info(int sig_num, siginfo_t *psiginfo, void *pcontext){
	printf("Recibi la senal #%d %s de %d, code: %d\n",
		sig_num, strsignal(sig_num), psiginfo->si_pid, psiginfo->si_code);
}

void catch_child(int sig_num, siginfo_t *psiginfo, void *pcontext){

	wait(NULL); /*Mandamos null porque en siginfo_t ya esta todo*/
	// catch_int_info(sig_num, psiginfo, pcontext);
	printf("Recibi la senal #%d %s de %d, code: %d\n",
		sig_num, strsignal(sig_num), psiginfo->si_pid, psiginfo->si_code);
}