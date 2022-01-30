#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <sys/mman.h>

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

	Signatures_t *p = mmap(NULL, sizeof(Signatures_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	assert(p != MAP_FAILED);

	pid_t pid = fork();

	if(pid == 0)
		hijo(p);
	else if(pid > 0){
		padre(p);
		wait(NULL);
	}else{
		// fork failed
		printf("fork failed\n");
		munmap(p, sizeof(Signatures_t));
		return 1;
	}

	printf("%s: Firmas: hijo: 0x%X	padre: 0x%X\n", (pid == 0) ? "HIJO" : "PADRE", p->hijo, p->padre);

	munmap(p, sizeof(Signatures_t));

	return 0;
}

/*
MAP_ANONYMOUS: parecido a un malloc, no necesita un file descriptor. No esta asociada a ningun nombre

Aca no hay data race porque los dos procesos fueron a escribir a dos direcciones de memoria distinta

Aca el fork esta hecho despues del mmap, asi que los dos proceos tienen las misma direccion de memoria, 
pero no tiene porque ser asi.
*/