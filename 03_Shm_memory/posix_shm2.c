#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define MM_SZ8 (1<<20)

typedef unsigned int u32;

int main()
{
    int shmfd;
    void *ptr;
    int cnt = 5;
    
    shmfd =  shm_open("/pru1", O_RDWR, 0640);
    assert(shmfd > 0);

    ptr = mmap(NULL, MM_SZ8, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    assert(ptr != MAP_FAILED);
    close(shmfd);


    while(cnt--) {
        ++(*((u32 *) ptr));
        printf("Val: 0x%08X\n", *((u32 *) ptr));
        getchar();
    }
    
    return 0;
}

