CXX = g++
DIR = $(notdir $(shell pwd))
CPPFLAGS = -I../
CXXFLAGS = -g -std=c++17 -fmax-errors=1
OBJ = $(patsubst %.cc, ../obj/%.$(DIR).o, $(wildcard *.cc))


all : $(OBJ) ../incltouch.x

../obj/%.$(DIR).o : %.cc 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@ -lpthread

../incltouch.x : incltouch.cpp graph.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@ -lstdc++fs
