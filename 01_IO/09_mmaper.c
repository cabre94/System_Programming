#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define MM_SZ8 (1<<20)

int main(int argc, char *argv[])
{
    int fd;
    int ret, i;
    int *ptr;
    
    printf("[%s] mi pid es %d\n", argv[0], getpid());
    
    fd = open("mm", O_RDWR | O_CREAT | O_TRUNC, 0640);
    assert(fd > 0);

    ret = ftruncate(fd, MM_SZ8);
    assert(ret != -1);

    ptr = mmap(NULL, MM_SZ8, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    assert(ptr != MAP_FAILED);

    puts("mmap done, paktc");
    getchar();
    
    close(fd);

    puts("close done, paktc");
    getchar();
    
    for( i = 0; i < MM_SZ8/sizeof(int); i++ )
        ptr[i] = i;
    
    puts("memfill done, paktc");
    getchar();

    munmap(ptr, MM_SZ8);
            
    puts("munmap done, paktc");
    getchar();

    return 0;
}

