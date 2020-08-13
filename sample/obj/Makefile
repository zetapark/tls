CXX = g++
OBJ = $(wildcard *.src.o)
EXE = $(patsubst %.x, ../%.x, $(wildcard *.x))
LDFLAGS = -L../../ -Wl,-rpath=../
LDLIBS = -lzeta -lzetatls -ljsoncpp -lstdc++fs -lrt -lmysqlcppconn -lpython3.6m

all : $(EXE)

../%.x : %.x
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

#    $@ |현재 목표 파일의 이름
#    $* | 확장자를 제외한 현재 목표 파일의 이름
#    $< | 현재 필수 조건 파일 중 첫 번째 파일 이름
#    $? | 현재 대상보다 최슨에 변경된 함수 조건 파일 이름
#    $^ | 현재 모든 필수 조건 파일들

