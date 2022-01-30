#pragma once

#include <mqueue.h>     /* message queue stuff */

mqd_t create_mq(const char* mq_name, long msg_sz, long msg_max);

mqd_t attach_mq(const char* mq_name);

int detach_mq(mqd_t mq);

void destroy_mq(const char* mq_name);

void send_messeges(mqd_t mq, int n_msgs, const char* prefix);

// int read_messege(mqd_t mq);

