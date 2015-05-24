all: server clean
server: server.o
	gcc server.o -pthread -o server
serv.o: server.c
	gcc -c server.c -o server.o
serv.c:
clean:
	rm *.o
