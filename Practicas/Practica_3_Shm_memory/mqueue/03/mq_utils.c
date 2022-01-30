#include "mq_utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <errno.h>
#include <unistd.h>
#include <mqueue.h>     /* message queue stuff */
#include <string.h>

mqd_t create_mq(const char* mq_name, long msg_sz, long msg_max){
	mqd_t mqd;
    struct mq_attr attr;        /* buffer for stat info */

	attr.mq_msgsize = msg_sz;
    attr.mq_maxmsg = msg_max;

	mqd = mq_open(mq_name, O_RDWR | O_CREAT | O_EXCL, 0600, &attr);

	if(-1 == (int) mqd){
		perror("Error en mq_open");
		exit(-1);
	}

	return mqd;
}

mqd_t attach_mq(const char* mq_name){
	return mq_open(mq_name, O_RDWR, 0600);
}

int detach_mq(mqd_t mq){
	return mq_close(mq);
}

void destroy_mq(const char* mq_name){
	mq_unlink(mq_name);
}

void send_messeges(mqd_t mq, int n_msgs, const char* prefix){
	char buff[64];
	for(int i=0; i < n_msgs; ++i){
		sprintf(buff, "%s - %d", prefix, i);
		mq_send(mq, buff, strlen(buff) + 1, i);
	}
}

// int read_messege(mqd_t mq){
// 	n = mq_receive(mqd, buff2, sizeof(buff2), NULL);
// 	if(n < 0){
// 		perror("Error mq_recieve");
// 	}
// 	printf("Recibidos %ld - message #%d -> %s\n",n,  i, buff2);

// } 
