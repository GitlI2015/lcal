CFLAGS=-Wall -lm -std=c99

CC=gcc

TARGET=lcal
SRCS=computer.c\
	 core.c

OBJS=$(SRCS:.c=.o)

%.o:%.c
	$(CC) -o $@ -c $< -std=c99

$(TARGET):$(OBJS)
#	@echo TARGET:$@
#	@echo OBJECTS:$^
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf $(TARGET) $(OBJS)

install:
	cp $(TARGET) /usr/bin/


