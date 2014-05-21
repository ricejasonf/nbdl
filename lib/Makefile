CC=g++ -g -std=c++11
CPP_FILES:=$(filter-out Entity.cpp, $(wildcard *.cpp))
OBJ_FILES:=$(CPP_FILES:.cpp=.o)

libnewhotness.a: Entity.o $(OBJ_FILES)
	ar -rcs libnewhotness.a Entity.o $(OBJ_FILES)

Entity.o: Entity.cpp Entity.h $(wildcard Validator*.h)
	$(CC) -c Entity.cpp

Validator.o: Validator.cpp Validator.h
	$(CC) -c Validator.cpp
	
BackEndPrint.o: BackEndPrint.cpp BackEndPrint.h
	$(CC) -c BackEndPrint.cpp

#%.o: %.cpp %.h
#	$(CC) -c $<

clean:
	rm *.o
