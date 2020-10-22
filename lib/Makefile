SHELL := /bin/bash
CXX = g++
OPTION = -j $(CPU)
CFLAG = -g -c -std=c++17 -fmax-errors=1 -pthread -DPYBIND11_PYTHON_VERSION=3.6m
INCLUDE = -I../ -I/usr/include/jsoncpp $(shell pkg-config --cflags python3)
LFLAG = -L. -Wl,-rpath=.
LIBS = -lgmpxx -lgmp -lnettle -ljsoncpp -lpthread -lhogweed -lstdc++fs -lrt -lmysqlcppconn -lpython3.8
DIR = $(notdir $(shell pwd))
OBJ = $(patsubst %.cc, ../obj/%.$(DIR).o, $(wildcard *.cc))
EXE = $(patsubst %.cpp, ../obj/%.$(DIR).x, $(wildcard *.cpp))
SH1 = $(wildcard obj/*.o)
SH2 = $(patsubst obj/%.tst.o, , $(SH1))

all : 
	../incltouch.x
	for i in $(shell ls -d */); do if [ $$i != 'obj/' ]; then cd $$i; make obj -f ../Makefile $(OPTION); cd ..; fi; done
	make libzeta.so 
	make middle.tls.x
	make catch.tst.x

obj : $(OBJ) $(EXE)

../obj/%.$(DIR).o : %.cc 
	$(CXX) -o $@ $(CFLAG) $(INCLUDE) $< -fPIC

../obj/%.$(DIR).x : %.cpp 
	$(CXX) $(CFLAG) $(INCLUDE) -o $@ $<

libzeta.so : $(SH2)
	$(CXX) -shared -o $@ $^ $(LIBS)

middle.tls.x : obj/middle.tls.x
	$(CXX) -o $@ $< -lzeta $(LFLAG)

catch.tst.x : obj/catch.tst.x $(wildcard obj/*.tst.o)
	$(CXX) -o $@ $^ -lzeta $(LIBS) $(LFLAG) 

clean :
	rm libzeta.so middle.tls.x catch.tst.x 
	rm obj/*.?

# incltouch will touch source files, according to inclusion of header files.
# incltouch generate tree of header inclusion.
# So it can track inclusion tree. 
# But there is one including rule.
# Any header file that is made by you and will be edited should use " ", not < >.
# Any header files that will not be edited by you should use < >. ie)standard library.
# And you should alwasy use directory structure.
# root dir - src directories.
#          |- src2 directory
#          |- src3 : src name can vary.
#          |- obj : object files will be placed here
#          |- tst : test files will be placed here
# And you should not add include path except -I../ and library include directory.
# In the end, the header inclusion will be like #include "src/header.h" when
# it is in the different directory.
# Or "header.h" when it is in the same directory.

