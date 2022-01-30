QUEUE_OBJS = Queque.o shmQueue.o
CFLAGS = -g -Wall 

default: all

all: 04_creator 04_put 04_get 04_monitor

# Queque.o: Queque.c Queque.h

# shmQueue.o: shmQueue.c shmQueue.h Queque.h

# 04_creator.o: 04_creator.c shmQueue.h Queque.h

04_creator: 04_creator.o $(QUEUE_OBJS)


# 04_put.o: 04_put.c shmQueue.h Queque.h

04_put: 04_put.o $(QUEUE_OBJS)


# 04_get.o: 04_get.c shmQueue.h Queque.h

04_get: 04_get.o $(QUEUE_OBJS)


# 04_monitor.o: 04_monitor.c shmQueue.h Queque.h

04_monitor: 04_monitor.o $(QUEUE_OBJS)



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
	rm -f 04_creator 04_get 04_monitor 04_put 


.%.d: %.c
	@set -e; $(CC) -MM -MG $(CFLAGS) $< | sed 's,^\($*\)\.o[ :]*,\1.o $@ : Makefile ,g' > $@