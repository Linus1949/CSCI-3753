CC = gcc
CFLAGS = -c -g -Wall -Wextra
LFLAGS = -Wall -Wextra -pthread

.PHONY: all clean

all: multi-lookup

multi-lookup: util.o queue.o multi-lookup.o  
	$(CC) $(LFLAGS) $^ -o $@

util.o: util.c util.h
	$(CC) $(CFLAGS) $<

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) $<

multi-lookup.o: multi-lookup.c multi-lookup.h
	$(CC) $(CFLAGS) $<

clean:
	rm -f multi-lookup
	rm -f *.o
	rm -f *~