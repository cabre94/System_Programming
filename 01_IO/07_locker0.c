#define _GNU_SOURCE 1

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    int fd;
    int ret;
    struct flock lk;
    
    printf("[%s] mi pid es %d\n", argv[0], getpid());
    
    fd = open("kk", O_RDWR | O_CREAT | O_TRUNC, 0640);
    printf("fd: %d\n", fd);
    assert(fd >= 0);
    
    ret = ftruncate(fd, 1<<20);
    assert(ret != -1);

    lk.l_type   = F_WRLCK;
    lk.l_whence = SEEK_SET;
    lk.l_start  = 0;
    lk.l_len    = 1<<10;
    puts("attempt to get write lock");
    ret = fcntl(fd, F_SETLKW, &lk);
    puts("got write lock");
    assert(ret >= 0);
    
    lk.l_type   = F_RDLCK;
    lk.l_whence = SEEK_SET;
    lk.l_start  = 1<<10;
    lk.l_len    = 1<<10;
    puts("attempt to get read lock");
    ret = fcntl(fd, F_SETLKW, &lk);
    puts("got read lock");
    assert(ret >= 0);
    
    puts("paktc");
    getchar();

    lk.l_type   = F_UNLCK;
    lk.l_whence = SEEK_SET;
    lk.l_start  = 0;
    lk.l_len    = 1<<10;
    ret = fcntl(fd, F_SETLK, &lk);
    puts("write lock released");
    assert(ret >= 0);
    
    puts("paktc");
    getchar();

    lk.l_type   = F_UNLCK;
    lk.l_whence = SEEK_SET;
    lk.l_start  = 1<<10;
    lk.l_len    = 1<<10;
    ret = fcntl(fd, F_SETLK, &lk);
    puts("read lock released");
    assert(ret >= 0);

    return 0;    
}







