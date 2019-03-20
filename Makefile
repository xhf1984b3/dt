#CC=arm-linux-gnueabihf-gcc
#CC=gcc
CFLAGS = -Wall -O -g
#INCLUDE = -I.
#LFLAGS =-L/
#CFLAGS=-DMEMWATCH -DMEMWATCH_STDIO

TARGET=dt-di-irq

SOUCE_FILES=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SOUCE_FILES))

%.o:%.c %.h
	$(CC) -c $(CFLAGS) $< -o $@ -lpthread 
	
%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@ -lpthread 
	
$(TARGET):$(OBJS)
	$(CC) $^ -o $@ $(INCLUDE) -lpthread
#98017: 98017.o xhf.o
#	$(CC) -g -o $@ $^
#%.o: %.c
#	$(CC) -g -c -o $@ $^

.PHONY:clean
clean:
	$(RM) $(TARGET) $(OBJS)

