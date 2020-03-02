CXX = g++
DIR = $(notdir $(shell pwd))
CPPFLAGS = -I../
CXXFLAGS = -g -c -std=c++17 -fmax-errors=1 -fpic
OBJ = $(patsubst %.cc, ../obj/%.$(DIR).o, $(wildcard *.cc))


all : $(OBJ) ../incltouch.x

../obj/%.$(DIR).o : %.cc 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@ 

../incltouch.x : incltouch.cpp graph.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@ -lstdc++fs
