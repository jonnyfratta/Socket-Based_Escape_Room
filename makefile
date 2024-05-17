# Makefile

all: server client other

server: server.o scenario.o server_utility.o
	gcc -Wall server.o scenario.o server_utility.o -o server

client: client.o client_utility.o
	gcc -Wall client.o client_utility.o -o client

other: client.o client_utility.o
	gcc -Wall client.o client_utility.o -o other

server.o: server.c
	gcc -Wall -g -c server.c -o server.o

client.o: client.c
	gcc -Wall -g -c client.c -o client.o
	
scenario.o: scenario.c scenario.h
	gcc -Wall -g -c scenario.c -o scenario.o

server_utility.o: server_utility.c server_utility.h
	gcc -Wall -g -c server_utility.c -o server_utility.o

client_utility.o: client_utility.c client_utility.h
	gcc -Wall -g -c client_utility.c -o client_utility.o
clean:
	rm *.o server client