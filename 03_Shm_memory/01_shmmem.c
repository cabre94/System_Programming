#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>

#define SIGNATURE_PADRE 0x5555
#define SIGNATURE_HIJO 0xAAAA

typedef struct {
	int padre;
	int hijo;
} Signatures_t;


void hijo(Signatures_t *pSig){
	pSig->hijo = SIGNATURE_HIJO;
}

void padre(Signatures_t *pSig){
	pSig->padre = SIGNATURE_PADRE;
}


int main(int argc, char const *argv[]){

	Signatures_t *p = calloc(1, sizeof(Signatures_t));
	assert(p);

	pid_t pid = fork();

	if(pid == 0)
		hijo(p);
	else if(pid > 0){
		padre(p);
		wait(NULL);
	}else{
		// fork failed
		printf("fork failed\n");
		free(p);
		return 1;
	}

	printf("%s: Firmas: hijo: 0x%X	padre: 0x%X\n", (pid == 0) ? "HIJO" : "PADRE", p->hijo, p->padre);
	free(p);

	return 0;

}

/*
No funca. Aunque alloque memoria dinamicamente, al hacer el fork, padre e hijo no se la
misma direccion de memoria, cada uno tiene SU copia. No es una memoria compaartida.
*/