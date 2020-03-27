CXX = g++
CPPFLAGS = -I../ -I/usr/include/jsoncpp
CXXFLAGS = -g -c -std=c++17 -fmax-errors=1
DIR = $(notdir $(shell pwd))
OBJ = $(patsubst %.cc, ../obj/%.$(DIR).o, $(wildcard *.cc))
FPIC = $(patsubst %.src.o, %.fpic.o, $(OBJ))
MAIN_OBJ = $(patsubst %.cpp, ../obj/%.$(DIR).x, $(wildcard *.cpp))

all : $(OBJ) $(MAIN_OBJ) $(OOBJ) $(FPIC)

../obj/%.$(DIR).o : %.cc 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

../obj/%.fpic.o : %.cc 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -fpic $< -o $@

../obj/%.$(DIR).x : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@
