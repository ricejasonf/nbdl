CC=g++ -g -std=c++11
NEWHOTNESS_LIBS=-lnewhotness -L/home/jason/Projects/NewHotness/lib
NEW_HOTNESS_INCLUDES=-I/home/jason/Projects/NewHotness/lib 

print: print.o Account.o
	$(CC) -o print print.o Account.o $(NEWHOTNESS_LIBS) -lboost_regex

print.o: print.cpp 
	$(CC) -c print.cpp $(NEW_HOTNESS_INCLUDES)

Account.o: Account.cpp Account.h
	$(CC) -c Account.cpp $(NEW_HOTNESS_INCLUDES)

clean:
	rm *.o 
