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
SRC=main.cpp \
	fsm.cpp

# object files
OBJ=$(SRC:.cpp=.o)

$(TARGET): $(OBJ) build
	$(CC) $(LINK_FLAGS) $(OBJ) -o build/$(TARGET)

%.o: %.c obj
	$(CC) $(C_FLAGS) $<

build: 
	mkdir build

clean: 
	rm *.o 
