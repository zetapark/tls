CFLAG = -c -g -std=c++17 -fmax-errors=1 -pthread
INCLUDE = -I/usr/include/jsoncpp -I../../lib -I/usr/include/python3.8 -I../../middle/src/ -I../../middle/pybind
DIR = $(notdir $(shell pwd))
CC = g++
OBJ = $(patsubst %.cc, ../obj/%.$(DIR).o, $(wildcard *.cc))
MAIN_OBJ = $(patsubst %.cpp, ../obj/%.$(DIR).x, $(wildcard *.cpp))

all : $(OBJ) $(MAIN_OBJ)

../obj/%.$(DIR).o : %.cc 
	$(CC) $(INCLUDE) $(CFLAG) $< -o $@

../obj/%.$(DIR).x : %.cpp
	$(CC) $(INCLUDE) $(CFLAG) $< -o $@
