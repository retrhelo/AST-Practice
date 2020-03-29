# 
# author: Yiming Liu
# 

TARGET=test

CC=g++

# compiler flags
C_FLAGS=-g -c

# linker flags
LINK_FLAGS=

# source files
SRC= \
src/main.cpp \
src/parser.cpp \
src/lexer.cpp  

OBJ= \
obj/main.o \
obj/parser.o \
obj/lexer.o 

default: $(OBJ) build
	$(CC) $(OBJ) -o build/$(TARGET)

tag: src inc
	ctags -R --language-force=C++ src/* inc/*

obj/main.o: src/main.cpp obj
	$(CC) $(C_FLAGS) src/main.cpp -o obj/main.o
obj/parser.o: src/parser.cpp obj
	$(CC) $(C_FLAGS) src/parser.cpp -o obj/parser.o
obj/lexer.o: src/lexer.cpp obj
	$(CC) $(C_FLAGS) src/lexer.cpp -o obj/lexer.o
build: 
	mkdir build
obj: 
	mkdir obj

clean: 
	rm -r obj
