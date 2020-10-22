CXX = g++
EXE = $(patsubst %.x, ../%.x, $(wildcard *.x))
LDFLAGS = -L../../lib/ -Wl,-rpath=../lib/
LDLIBS = -lzeta -ljsoncpp -lpthread -lstdc++fs -lrt -lmysqlcppconn -lnettle -lgmp -lgmpxx
#filesystem library should be next to zeta library

all : $(EXE)

../adnet2.src.x : adnet.src.o adnet2.src.o adnet2.src.x
	$(CXX) -o $@ $^ $(LDLIBS) $(LDFLAGS)

../%.x : %.x %.o
	$(CXX) -o $@ $^ $(LDLIBS) $(LDFLAGS)

#    $@ |현재 목표 파일의 이름
#    $* | 확장자를 제외한 현재 목표 파일의 이름
#    $< | 현재 필수 조건 파일 중 첫 번째 파일 이름
#    $? | 현재 대상보다 최슨에 변경된 함수 조건 파일 이름
#    $^ | 현재 모든 필수 조건 파일들

