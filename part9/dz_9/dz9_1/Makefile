send: main.c libsendfile.a
	gcc -o send main.c -L. -lsendfile

libsendfile.a: sendfile.o
	ar cr libsendfile.a sendfile.o

sendfile.o: sendfile.c sendfile.h
	gcc -c sendfile.c

