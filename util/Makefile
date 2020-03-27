CXX = g++
DIR = $(notdir $(shell pwd))
CPPFLAGS = -I../
CXXFLAGS = -g -std=c++17 -fmax-errors=1
OBJ = $(patsubst %.cc, ../obj/%.$(DIR).o, $(wildcard *.cc))
FPIC = $(patsubst %.$(DIR).o, %.fpic.o, $(OBJ))


all : $(OBJ) ../incltouch.x $(FPIC)

../obj/%.$(DIR).o : %.cc 
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@ 

../obj/%.fpic.o : %.cc 
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -fpic $< -o $@

../incltouch.x : incltouch.cpp graph.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@ -lstdc++fs
