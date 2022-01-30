#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

typedef struct {
	double t;
	double val;
} str_t;

int main()
{
	int fd, nc, tot;
	str_t mys = { 0, 0 };

	fd = open("kk_open", O_RDONLY);
	if( fd < 0 ) {
		perror("open de kk_open");
		exit(1);
	}

	tot = 0;

	while( 1 ) {
		nc = read(fd, &mys, sizeof(mys));
		if( nc == 0 )
			break;
		if( nc < 0 ) {
			perror("reading from kk_open" );
			exit(1);
		}
		if( nc < sizeof(mys) ) {
			printf("Estructura corrupta\n");
			exit(2);
		}
		
		printf("%d :: T = %lf  Val=%lf \n", tot, mys.t, mys.val);
		tot++;
	}

	close(fd);

	return 0;
}
