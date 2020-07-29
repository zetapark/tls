CXX = g++
CPPFLAGS = -I../ -I/usr/include/jsoncpp
CXXFLAGS = -g -c -std=c++17 -fmax-errors=1 -fpic
DIR = $(notdir $(shell pwd))
OBJ = $(patsubst %.cc, ../obj/%.$(DIR).o, $(wildcard *.cc))


all : $(OBJ) 

../obj/%.$(DIR).o : %.cc 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@
