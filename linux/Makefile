CFLAGS=-c -Wall -O2 -I../src -D__LINUX__
LIBS = -lm

all: demo

demo: main.o Makefile
	$(CC) main.o $(LIBS) -o demo

main.o: main.c ../src/g4enc.inl ../src/G4ENCODER.h Makefile
	$(CC) $(CFLAGS) main.c

clean:
	rm *.o demo

