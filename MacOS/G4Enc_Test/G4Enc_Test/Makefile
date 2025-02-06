CFLAGS=-D__LINUX__ -Wall -O2 
LIBS = 

all: g4enctest

g4enctest: main.o
	$(CC) main.o $(LIBS) -o g4enctest 

main.o: G4Enc_Test/G4Enc_Test/main.cpp
	$(CXX) $(CFLAGS) -c G4Enc_Test/G4Enc_Test/main.cpp

clean:
	rm -rf *.o g4enctest
