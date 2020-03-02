CXX = g++
TOBJ = $(wildcard *.tst.o)
SHARED_OBJ = $(wildcard *.tcpip.o)
SHARED_OBJ2 = $(wildcard *.database.o)
DNDD = $(wildcard *.dndd.o)
SITE = $(wildcard *.site_src.o)
OBJ = $(wildcard *.src.o)
UTIL = $(wildcard *.util.o)
OBJ += $(UTIL)
EXE = $(wildcard *.src.x)
LDFLAGS = -L.. -Wl,-rpath=.
LDLIBS = -lgmpxx -lgmp -lnettle -ljsoncpp -lpthread -lhogweed -lzeta -lstdc++fs
#filesystem library should be next to zeta library

all : ../libzeta.so ../libzetadb.so $(EXE) ../catch.tst.x ../dndd.dndd.x

../catch.tst.x : catch.tst.x $(TOBJ) $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

../%.x : %.x $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

../libzeta.so : $(SHARED_OBJ) 
	$(CXX) -shared -o $@ $^

../libzetadb.so : $(SHARED_OBJ2)
	$(CXX) -shared -o $@ $^

../dndd.dndd.x : $(DNDD) $(UTIL) dndd.dndd.x
	$(CXX) $(LDFLAGS) -o $@ $^ -lzetadb -lmysqlcppconn $(LDLIBS)

#    $@ |현재 목표 파일의 이름
#    $* | 확장자를 제외한 현재 목표 파일의 이름
#    $< | 현재 필수 조건 파일 중 첫 번째 파일 이름
#    $? | 현재 대상보다 최슨에 변경된 함수 조건 파일 이름
#    $^ | 현재 모든 필수 조건 파일들

