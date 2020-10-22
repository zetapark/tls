CXX = g++
OBJ = $(wildcard *.src.o)
EXE = $(patsubst %.x, ../%.x, $(wildcard *.x))
LDFLAGS = -L../ -L../../lib/ -Wl,-rpath=../lib/ -Wl,-rpath=.
LDLIBS = -lgmpxx -lgmp -lnettle -ljsoncpp -lpthread -lhogweed -lzeta -lstdc++fs -lrt -lmysqlcppconn
OPENCV = $(shell pkg-config opencv4 --libs)
#filesystem library should be next to zeta library

all : ../libbiz.so $(EXE)

../libbiz.so : $(wildcard *.o)
	$(CXX) -o $@ $^ -shared $(LDLIBS) $(LDFLAGS)

../%.x : %.x
	$(CXX) -o $@ $^ -lbiz -lzeta $(LDLIBS) $(OPENCV) $(LDFLAGS)
#    $@ |현재 목표 파일의 이름
#    $* | 확장자를 제외한 현재 목표 파일의 이름
#    $< | 현재 필수 조건 파일 중 첫 번째 파일 이름
#    $? | 현재 대상보다 최슨에 변경된 함수 조건 파일 이름
#    $^ | 현재 모든 필수 조건 파일들

