CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGETS = server client

all: $(TARGETS)

server: server.o
	$(CC) $(CFLAGS) -o $@ $<

client: client.o
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.c common.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean
