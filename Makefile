CFLAGS=-Wall -lm
CC=gcc

TARGET=lcal
SRCS=computer.c\
	 core.c

OBJS=$(SRCS:.c=.o)

%.o:%.c
	$(CC) -o $@ -c $<

$(TARGET):$(OBJS)
#	@echo TARGET:$@
#	@echo OBJECTS:$^
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf $(TARGET) $(OBJS)

install:
	cp $(TARGET) /usr/bin/


