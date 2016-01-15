CFLAGS=-Wall -lm
CC=clang

TARGET=mcm
SRCS=computer.c\
	 core.c
INC=-I./

OBJS=$(SRCS:.c=.o)

$(TARGET):$(OBJS)
#	@echo TARGET:$@
#	@echo OBJECTS:$^
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(TARGET) $(OBJS)

%.o:%.c
	$(CC) -o $@ -c $<
