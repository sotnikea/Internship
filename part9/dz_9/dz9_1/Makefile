send: main.o libsendfile.a
	gcc -o send main.o -L. -lsendfile

main.o: main.c sendfile.h
	gcc -c main.c

libsendfile.a: sendfile.o
	ar cr libsendfile.a sendfile.o

sendfile.o: sendfile.c sendfile.h
	gcc -c sendfile.c
