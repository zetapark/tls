CFLAG = -c -O3 -std=c++17 -fmax-errors=1 -pthread
INCLUDE = -I../../lib/ -I/usr/include/jsoncpp -I/usr/include/opencv2 -I/usr/include/python3.8 $(shell pkg-config opencv4 --cflags) -I/usr/include/opencv4/opencv2
DIR = $(notdir $(shell pwd))
CC = g++
OBJ = $(patsubst %.cc, ../obj/%.$(DIR).o, $(wildcard *.cc))
MAIN_OBJ = $(patsubst %.cpp, ../obj/%.$(DIR).x, $(wildcard *.cpp))

all : $(OBJ) $(MAIN_OBJ)

../obj/%.$(DIR).o : %.cc 
	$(CC) $(INCLUDE) $(CFLAG) $< -o $@ -fpic

../obj/%.$(DIR).x : %.cpp
	$(CC) $(INCLUDE) $(CFLAG) $< -o $@
