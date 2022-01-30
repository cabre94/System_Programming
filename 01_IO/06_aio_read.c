#include <sys/types.h> 
#include <unistd.h> 
#include <fcntl.h>
#include <aio.h> 

#include <errno.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <memory.h> 

int main(int argc, char *argv[]){

    char buf[4096]; 
    ssize_t ret;
    ssize_t nbytes; 
    int j;

    struct aiocb myaiocb;
    
    for( j = 0; j < 5; j++) {
        memset(&myaiocb, 0, sizeof(struct aiocb));
        
        myaiocb.aio_fildes = open( "/dev/zero", O_RDONLY); 
        myaiocb.aio_offset = 0; 
        myaiocb.aio_buf = (void *) buf; 
        myaiocb.aio_nbytes = sizeof (buf); 
        myaiocb.aio_sigevent.sigev_notify = SIGEV_NONE; 
        
        ret = aio_read( &myaiocb );
        if( ret )
            perror("aio_read:"); 
        
        /* continue processing */ 
        //...  
        /* wait for completion */ 
        
        while( (ret = aio_error( &myaiocb)) == EINPROGRESS )
            puts("esperando"); 
        /* free the aiocb */ 
        nbytes = aio_return( &myaiocb);
        
        printf("aio_return: %lu bytes\n", nbytes);
    }
    
    return 0;
}
