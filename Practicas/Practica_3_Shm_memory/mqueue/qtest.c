#include <mqueue.h>     /* message queue stuff */
#define MQ_DEF_MSGSIZE 32
#define MQ_DEF_MAXMSG 10
#include <unistd.h>     /* for getopt() */
#include <errno.h>      /* errno and perror */
#include <fcntl.h>      /* O_flags */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <unistd.h>


static void doChild(mqd_t mqd)
{
    int c = 10;
    char buff[32];
    int i;
    for(i = 0; i < c; i++) {
        mq_receive(mqd, buff, sizeof(buff), NULL);
        printf("message #%d -> %s\n", i, buff);
    }
    mq_close(mqd);
}

static void doParent(mqd_t mqd)
{
    int c = 10;
    char buff[32];
    int i;

    for(i = 0; i < c; i++) {
        sprintf(buff, "message: %d", i);
        mq_send(mqd, buff, strlen(buff) + 1, i);
    }
    mq_close(mqd);
}

int main()
{
    mqd_t mqd;
    struct mq_attr attr;        /* buffer for stat info */
    
    attr.mq_msgsize = MQ_DEF_MSGSIZE;
    attr.mq_maxmsg = MQ_DEF_MAXMSG;
    int pid;



    mqd = mq_open("/test", O_RDWR | O_CREAT | O_EXCL, 0600, &attr);

    if( -1 != (int) mqd ) {
        if ( ! mq_getattr(mqd,&attr) ) {
            printf("flags: 0x%x  maxmsg: %d  msgsize: %d  curmsgs: %d\n",
                   (int) attr.mq_flags, (int) attr.mq_maxmsg, (int) attr.mq_msgsize, (int) attr.mq_curmsgs);
        }
        else {
            perror("mq_getattr()");
            return EXIT_FAILURE;
        }
    } else {
        perror("sq_open");
        return EXIT_FAILURE;
    }

    pid = fork();
    if( pid  == -1) {
        perror("fork");
        return EXIT_FAILURE;
    } else if(pid == 0) {
        doChild(mqd);
    } else {
        int status;
        doParent(mqd);
        waitpid(pid, &status, 0);
        mq_unlink("/test");    
    }
    return EXIT_SUCCESS;
}

