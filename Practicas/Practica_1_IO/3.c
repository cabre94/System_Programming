#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> /* exit */
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

void list_wSizes(DIR* dirp);

int main(){

	DIR* dirp = opendir(".");

	if(dirp == NULL){
		perror("Parent Directory");
		exit(1);
	}

	list_wSizes(dirp);
	
	closedir(dirp);

	return 0;
}

void list_wSizes(DIR* dirp){
	struct dirent *cd;
	struct stat sb;

	// Aca creo que ya puedo asumir que el directory stream descriptor es valido
	// errno = 0;	// Para diferenciar NULL de fallo y end of stream
	cd = readdir(dirp);


	while (cd != NULL){
		if (cd->d_name[0] != '.'){
			if (lstat(cd->d_name, &sb) == -1) {
				perror("lstat");
				// exit(EXIT_FAILURE);
			}

			printf("%s\t", cd->d_name);
			printf(" %jd\n", sb.st_size);

			if(cd->d_type == DT_DIR){
				DIR* aux_dir = opendir(cd->d_name);
				chdir(cd->d_name);				// Medio rancio
				list_wSizes(aux_dir);
				closedir(aux_dir);
				chdir("..");
			}
		}

		// printf("%s\t%d\n", cd->d_name, cd->d_type);


		cd = readdir(dirp);
	}



}
