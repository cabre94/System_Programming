#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>

typedef struct {
	double t;
	double val;
} str_t;

int search(str_t *result, str_t *pstart, int nrecs);

int main()
{
	struct stat st;
    str_t *ptr;
    str_t mys;
    int ret;
	int fd;
	
	fd = open("kk_open", O_RDWR);
    assert(fd > 0);

	ret = fstat(fd, &st);
    assert(ret != -1);

    ptr = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    assert(ptr != MAP_FAILED);

    close(fd);
	getchar();

    mys.t = 2.3;
	ret = search(&mys, ptr, st.st_size/sizeof(str_t));
	assert(ret == 0);
    
	printf("T = %lf  Val=%lf \n",  mys.t, mys.val);

    munmap(ptr, st.st_size);

	return 0;
}

int search(str_t *result, str_t *p, int nrecs)
{
    int fin = nrecs - 1,
        ini = 0,
        mid;

    if( result->t < p[ini].t || result->t > p[fin].t ) {
        printf ("Tiempo fuera de rango\n");
        return -1;
    }

    while( fin - ini > 1 ) {
        mid = (ini+fin) >> 1;

        if( p[mid].t == result->t ) {
            result->val = p[mid].val;
            return 0;
        }

        if( p[mid].t > result->t )
            fin = mid;
        else
            ini = mid;
    }
    result->val  = p[mid].val;
    result->val += ((p[mid+1].val-p[mid].val)/(p[mid+1].t-p[mid].t)) *(result->t-p[mid].t);
    return 0;
}

