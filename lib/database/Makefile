CFLAG = -g -c -std=c++14 -fmax-errors=1 -pthread -fPIC
INCLUDE = -I../ -I../src/  -I/usr/include/jsoncpp
DIR = $(notdir $(shell pwd))
CC = g++
OBJ = $(patsubst %.cc, ../obj/%.$(DIR).o, $(wildcard *.cc))

all : $(OBJ)

../obj/%.$(DIR).o : %.cc 
	$(CC) $(INCLUDE) $(CFLAG) $< -o $@
