SHELL := /bin/bash
CXX = g++
OPTION = -j $(CPU)
CFLAG = -O3 -c -std=c++17 -fmax-errors=1 -pthread -DPYBIND11_PYTHON_VERSION=3.6m
INCLUDE = -I../ -I/usr/include/jsoncpp -I/usr/include/python3.6m
LFLAG = -L. -Wl,-rpath=.
LIBS = -lgmpxx -lgmp -lnettle -ljsoncpp -lpthread -lhogweed -lstdc++fs -lrt -lmysqlcppconn -lpython3.6m
DIR = $(notdir $(shell pwd))
OBJ = $(patsubst %.cc, ../obj/%.$(DIR).o, $(wildcard *.cc))
EXE = $(patsubst %.cpp, ../obj/%.$(DIR).x, $(wildcard *.cpp))
SH1 = $(wildcard obj/*.o)
SH2 = $(patsubst obj/%.tst.o, , $(SH1))
SH3 = $(patsubst obj/%.pybind.o, , $(SH2))

all : 
	../incltouch.x
	for i in $(shell ls -d */); do if [ $$i != 'obj/' ]; then cd $$i; make obj -f ../Makefile $(OPTION); cd ..; fi; done
	make libzeta.so 
	make z.so 
	make middle.tls.x
	make catch.tst.x

obj : $(OBJ) $(EXE)

../obj/%.$(DIR).o : %.cc 
	$(CXX) $(CFLAG) $(INCLUDE) -o $@ $< -fPIC

../obj/%.$(DIR).x : %.cpp 
	$(CXX) $(CFLAG) $(INCLUDE) -o $@ $<

libzeta.so : $(SH3)
	$(CXX) -shared -o $@ $^ $(LIBS)

z.so : $(wildcard obj/*.pybind.o)
	$(CXX) -shared -o $@ $^ -lzeta -Wl,-rpath=. -L.

middle.tls.x : obj/middle.tls.x
	$(CXX) -o $@ $< -lzeta $(LFLAG) 

catch.tst.x : obj/catch.tst.x $(wildcard obj/*.tst.o)
	$(CXX) -o $@ $^ $(LIBS) $(LFLAG) 

clean :
	rm libzeta.so middle.tls.x catch.tst.x z.so
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

