CXX = g++
DIR = $(notdir $(shell pwd))
CPPFLAGS = -I../
CXXFLAGS = -g -std=c++17 -fmax-errors=1 -fpic
OBJ = $(patsubst %.cc, ../obj/%.$(DIR).o, $(wildcard *.cc))


all : $(OBJ) 

../obj/%.$(DIR).o : %.cc 
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@ 

