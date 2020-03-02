CXX = g++
TOBJ = $(wildcard *.tst.o)
SITE = $(wildcard *.site_src.o)
OBJ = $(wildcard *.src.o)
UTIL = $(wildcard *.util.o)
OBJ += $(UTIL)
EXE = $(patsubst %.x, ../%.x, $(wildcard *.x))
LDFLAGS = -L.. -Wl,-rpath=.
LDLIBS = -lgmpxx -lgmp -lnettle -ljsoncpp -lpthread -lhogweed -lzeta -lstdc++fs
DB = -lzetadb -lmysqlcppconn
#filesystem library should be next to zeta library

all : ../libzeta.so ../libzetadb.so $(EXE) ../catch.tst.x ../dndd.dndd.x

../catch.tst.x : catch.tst.x $(TOBJ) $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

../%.src.x : %.src.x $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

../%.site_src.x : %.site_src.x $(UTIL)
	$(CXX) $(LDFLAGS) -o $@ $^ $(DB) $(LDLIBS) 

../libzeta.so : $(wildcard *.tcpip.o) 
	$(CXX) -shared -o $@ $^

../libzetadb.so : $(wildcard *.database.o)
	$(CXX) -shared -o $@ $^

../dndd.dndd.x : $(wildcard *.dndd.o) $(UTIL) dndd.dndd.x
	$(CXX) $(LDFLAGS) -o $@ $^ $(DB) $(LDLIBS)

#    $@ |현재 목표 파일의 이름
#    $* | 확장자를 제외한 현재 목표 파일의 이름
#    $< | 현재 필수 조건 파일 중 첫 번째 파일 이름
#    $? | 현재 대상보다 최슨에 변경된 함수 조건 파일 이름
#    $^ | 현재 모든 필수 조건 파일들

