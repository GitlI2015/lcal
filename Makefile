dbg= 
CFLAGS=-Wall -lm -std=c99 -lreadline
DEBGUG=

ifeq ($(dbg),true)
	DEBUG=-g
endif

CC=gcc

TARGET=lcal
SRCS=computer.c\
	 core.c

OBJS=$(SRCS:.c=.o)

%.o:%.c
	$(CC) $(DEBUG) -o $@ -c $< -std=c99  

$(TARGET):$(OBJS)
#	@echo TARGET:$@
#	@echo OBJECTS:$^
	$(CC) $(DEBUG) -o $@ $^ $(CFLAGS)

clean:
	rm -rf $(TARGET) $(OBJS)

install:
	cp $(TARGET) /usr/bin/


