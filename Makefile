CC=g++
CFLAGS=-ggdb -std=c++11 -Wall -pedantic

all: client server

client: client-main.c client.h client.c
	$(CC) $(CFLAGS) -o client client-main.c client.c

server: server-main.c server.h server.c
	$(CC) $(CFLAGS) -o server server-main.c server.c

clean:
	rm -rf client server
