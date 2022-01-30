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
	int i,fd;
	str_t mys = { 0, 0 };
				
	fd = open("kk_open", O_RDWR | O_CREAT, 0640);
	if( fd < 0 ) {
		perror("open de kk_open");
		exit(1);
	}
		
	for( i = 0; i < 10000; i++ ) {
		mys.t += 0.1*(rand()%10);
		mys.val = sin(mys.t);
		write(fd, &mys, sizeof(mys));
	}

	close(fd);

	return 0;
}
