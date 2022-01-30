#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>

typedef struct{
	double t;
	double val;
} str_t;

int search_seek(double t, str_t *result, int fd);
str_t get_record(int fd, int pos);
int get_nrecords(int fd);

int main(){

	int fd, ret;
	str_t mys = {0};

	fd = open("kk_open", O_RDWR, 0640);
	if (fd < 0){
		perror("open de kk_open");
		exit(1);
	}

	ret = search_seek(2.3, &mys, fd);
	assert(ret == 0);
	printf("T = %lf  Val=%lf \n", mys.t, mys.val);

	close(fd);

	return 0;
}

int search_seek(double t, str_t *result, int fd)
{
	int ini = 0, fin, mid;
	str_t s1, s2;

	fin = get_nrecords(fd) - 1;
	s1 = get_record(fd, ini);
	s2 = get_record(fd, fin);

	if (t < s1.t || t > s2.t)
	{
		printf("Tiempo fuera de rango\n");
		return (-1);
	}

	while (fin - ini > 1)
	{
		mid = (ini + fin) / 2;

		*result = get_record(fd, mid);

		if (result->t == t)
			return 0;

		if (result->t > t)
		{
			fin = mid;
			s2 = get_record(fd, fin);
		}
		else
		{
			ini = mid;
			s1 = get_record(fd, ini);
		}
	}
	result->t = t;
	result->val = s1.val;
	result->val += ((s2.val - s1.val) / (s2.t - s1.t)) * (t - s1.t);
	return 0;
}

str_t get_record(int fd, int pos)
{
	int nc;
	str_t result;
	lseek(fd, pos * sizeof(str_t), SEEK_SET);
	nc = read(fd, &result, sizeof(result));
	assert(nc == sizeof(result));
	return result;
}

int get_nrecords(int fd)
{
	struct stat s;
	int ret;

	ret = fstat(fd, &s);
	assert(ret != -1);

	return s.st_size / sizeof(str_t);
}
