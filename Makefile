CC = gcc
CFLAGS = -Wall -Wextra
PTHREAD = -pthread

all: server client

server: src/server.c
	$(CC) $(CFLAGS) src/server.c -o server $(PTHREAD)

client: src/client.c
	$(CC) $(CFLAGS) src/client.c -o client

clean:
	rm -f server client received_*
