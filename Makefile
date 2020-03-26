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
SRC=

default: $(OBJ) build
	$(CC) $< -o build/$(TARGET)

tag: src inc
	ctags -R --language-force=C++ src/* inc/*

build: 
	mkdir build
src: 
	mkdir src
inc: 
	mkdir inc

clean: 
	rm -r obj
