#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>


int main(){

	const int SZ = 64;
	char msg[SZ];

	while(1){
		memset(msg, 0, SZ);

		if(read(STDIN_FILENO, msg, SZ) == -1){
			perror("read");
		}

		for(int i=0; i < strlen(msg); ++i)
			msg[i] = toupper(msg[i]);
		
		printf("Lei. Ahora escribo");

		if(write(STDOUT_FILENO, msg, SZ) == -1){
			perror("write");
		}


	}



	return 0;
}