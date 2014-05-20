CC=g++ -g -std=c++11
LIB_FILE=../../lib/newhotness.a

print: print.o $(LIB_FILE)
	$(CC) -o print print.o $(LIB_FILE)

print.o: print.cpp
	$(CC) -c print.cpp

clean:
	rm *.o 
