CC=g++ -g -std=c++11
LIB_DIR=../../lib
LIB_CPP_FILES:=$(wildcard $(LIB_DIR)/*.cpp)
LIB_OBJ_FILES:=$(LIB_CPP_FILES:.cpp=.o)

print: print.o $(LIB_OBJ_FILES)
	$(CC) -o print print.o $(LIB_OBJ_FILES)

print.o: print.cpp
	$(CC) -c print.cpp

$(LIB_DIR)/%.o: $(LIB_DIR)/%.cpp
	$(CC) -c $< -o $@
