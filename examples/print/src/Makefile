CC=g++ -g -std=c++11
NEW_HOTNESS_LIBS=-lnewhotness -L/home/jason/Projects/NewHotness/lib
NEW_HOTNESS_INCLUDES=-I/home/jason/Projects/NewHotness/lib 
#NEW_HOTNESS_LIBS=-lnewhotness -L../../lib
#NEW_HOTNESS_INCLUDES=-I../../lib 

print: print.o Account.o
	$(CC) -o print print.o Account.o $(NEW_HOTNESS_LIBS) -lboost_regex


print.o: print.cpp 
	$(CC) -c print.cpp $(NEW_HOTNESS_INCLUDES)

Account.o: Account.cpp Account.h
	$(CC) -c Account.cpp $(NEW_HOTNESS_INCLUDES)

print.js: print.cpp Account.cpp
	/usr/local/src/emscripten/em++ -g -std=c++11 -o print.js print.cpp Account.cpp $(NEW_HOTNESS_INCLUDES) $(NEW_HOTNESS_LIBS) -lboost_regex

clean:
	rm *.o 
