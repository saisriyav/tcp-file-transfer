CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
PTHREAD = -pthread
LIBS = -lcrypto

all: server client

server: src/server.c src/network.c src/utils.c src/crypto.c
	$(CC) $(CFLAGS) src/server.c src/network.c src/utils.c src/crypto.c -o server $(PTHREAD) $(LIBS)

client: src/client.c src/network.c src/utils.c src/crypto.c
	$(CC) $(CFLAGS) src/client.c src/network.c src/utils.c src/crypto.c -o client $(LIBS)

clean:
	rm -f server client received_*
