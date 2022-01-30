#include <mqueue.h>     /* message queue stuff */
#include <unistd.h>     /* for getopt() */
#include <errno.h>      /* errno and perror */
#include <fcntl.h>      /* O_flags */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>



#define MQ_DEF_MSGSIZE 32
#define MQ_DEF_MAXMSG 10




int main(){
	
	const long MQ_MSGSIZE 	= 32;
	const long MQ_MAXMSG 	= 10;

	mqd_t mqd;
	struct mq_attr attr;

	// attr.mq_flags	= ;		// Flags (ignored for mq_open())
	attr.mq_maxmsg 	= MQ_MAXMSG;		// Max. # of messages on queue
	attr.mq_msgsize = MQ_MSGSIZE;	// Max. message size (bytes)
	// attr.mq_curmsgs = ;		// # of messages currently in queue (ignored for mq_open())

	mqd = mq_open("/mq_test", O_RDWR | O_CREAT | O_TRUNC, 0600, &attr);


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



	struct mq_attr new_attr;

	new_attr.mq_flags	= O_NONBLOCK;		// Flags (ignored for mq_open())
	new_attr.mq_maxmsg 	= 20L;		// Max. # of messages on queue
	new_attr.mq_msgsize = 64L;	// Max. message size (bytes)
	new_attr.mq_curmsgs = 10;		// # of messages currently in queue (ignored for mq_open())




	int ret = mq_setattr(mqd, &new_attr, &attr);


	if(ret == -1){
		perror("Error en mq_setattr");
		exit(EXIT_FAILURE);
	}

	if ( ! mq_getattr(mqd,&new_attr) ) {
		printf("flags: 0x%x  maxmsg: %d  msgsize: %d  curmsgs: %d\n",
				(int) new_attr.mq_flags, (int) new_attr.mq_maxmsg, (int) new_attr.mq_msgsize, (int) new_attr.mq_curmsgs);
	}
	else {
		perror("mq_getattr()");
		return EXIT_FAILURE;
	}





	return 0;
}