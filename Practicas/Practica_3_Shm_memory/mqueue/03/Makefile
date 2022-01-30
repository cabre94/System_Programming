# QUEUE_OBJS = Queque.o mq_utils.o
QUEUE_OBJS = mq_utils.o
CFLAGS = -g -Wall 

default: all

all: mq_creator mq_producer mq_consumer launcher

mq_creator: mq_creator.o $(QUEUE_OBJS)

mq_producer: mq_producer.o $(QUEUE_OBJS)

mq_consumer: mq_consumer.o $(QUEUE_OBJS)

launcher: launcher.o $(QUEUE_OBJS)



DEPS := $(patsubst %.c,.%.d, $(wildcard *.c))


no-deps-targets := clean rclean

ifneq ($(filter $(no-deps-targets), $(MAKECMDGOALS)),)
	ifeq ($(filter-out $(no-deps-targets), $(MAKECMDGOALS)),)
		inc-deps := 0
	endif
endif


ifneq ($(inc-deps), 0)
-include $(DEPS)
endif



clean:
	rm -f core* *.o .*.d

rclean: clean
	rm -f mq_creator mq_producer mq_consumer launcher  


.%.d: %.c
	@set -e; $(CC) -MM -MG $(CFLAGS) $< | sed 's,^\($*\)\.o[ :]*,\1.o $@ : Makefile ,g' > $@