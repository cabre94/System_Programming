#define _GNU_SOURCE 1

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    int fd, fdup;
    int ret;
    
    printf("[%s] mi pid es %d\n", argv[0], getpid());
    
    fd = open("kk", O_RDWR | O_CREAT, 0640);
    printf("fd: %d\n", fd);
    assert(fd >= 0);
    
    ret = ftruncate(fd, 1<<20);
    assert(ret != -1);

    fdup = fcntl(fd, F_DUPFD, 7);
    printf("fdup: %d\n", fdup);
    assert(fdup >= 0);
    
    ret = fcntl(fd, F_GETFD);
    assert(ret != -1);
    printf("F_GETFD retorno 0x%x\n", ret);
    
    ret = fcntl(fd, F_SETFD, FD_CLOEXEC);
    assert(ret != -1);
    
    ret = fcntl(fd, F_GETFD);
    assert(ret != -1);
    printf("F_GETFD retorno 0x%x\n", ret);
    
    ret = fcntl(fd, F_GETFL);
    assert(ret != -1);
    printf("F_GETFL retorno 0x%x\n", ret);
    
    puts("paktc");
    getchar();
    
    close(fd);
    close(fdup);
    
    puts("paktc");
    getchar();
    
    return 0;    
}







