CC=g++ -g -std=c++11
NEWHOTNESS_LIBS=-lnewhotness -L/home/jason/Projects/NewHotness/lib
NEW_HOTNESS_INCLUDES=-I/home/jason/Projects/NewHotness/lib 

print: print.o
	$(CC) -o print print.o $(NEWHOTNESS_LIBS)

print.o: print.cpp
	$(CC) -c print.cpp $(NEW_HOTNESS_INCLUDES)

clean:
	rm *.o 
