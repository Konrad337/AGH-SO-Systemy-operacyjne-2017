LIBST = ../task1/libstat.a
LIBSH = ../task1/libstat.a

.PHONY: clean

static: task2.c

	gcc -c -Wall task2.c -o program.o
	gcc program.o -o program $(LIBST)
	rm program.o

shared: task2.c

	gcc -c -Wall -fPIC task2.c -o program.o
	gcc program.o -o program $(LIBSH)
	rm program.o

clean:
	rm -f *.a
	rm -f *.o
	rm -f *.so
