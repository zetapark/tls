CXX = g++
TOBJ = $(wildcard *.tst.o)
OBJ = $(wildcard *.src.o)
EXE = $(patsubst %.x, ../%.x, $(wildcard *.x))
LDFLAGS = -L../../ -Wl,-rpath=../
LDLIBS = -lgmpxx -lgmp -lnettle -ljsoncpp -lpthread -lhogweed -lzeta -lstdc++fs -lrt -lmysqlcppconn
#filesystem library should be next to zeta library

all : ../catch.tst.x ../middle.src.x ../libzetatls.so ../zeta.so

../catch.tst.x : catch.tst.x $(TOBJ) $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

../%.src.x : %.src.x $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

../libzetatls.so : $(wildcard *.fpic.o) 
	$(CXX) -o $@ $^ -shared $(LDLIBS) $(LDFLAGS)

../zeta.so : $(wildcard *.pybind.o)
	$(CXX) -o $@ $^ -shared $(LDLIBS) -lzetatls $(LDFLAGS) 

#    $@ |현재 목표 파일의 이름
#    $* | 확장자를 제외한 현재 목표 파일의 이름
#    $< | 현재 필수 조건 파일 중 첫 번째 파일 이름
#    $? | 현재 대상보다 최슨에 변경된 함수 조건 파일 이름
#    $^ | 현재 모든 필수 조건 파일들

