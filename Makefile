CC=clang++
CFLAGS=-c -Wall -std=c++11 -stdlib=libc++ -O3 -I/usr/local/Cellar/boost/1.52.0/include
LDFLAGS=-stdlib=libc++
SRC=fibonacciheap.cpp
OBJ=$(SRC:.cpp=.o)

all: depend fibonacciheap

depend: .depend

.depend: $(SRC)
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^ > ./.depend;

include .depend

fibonacciheap: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o fibonacciheap
