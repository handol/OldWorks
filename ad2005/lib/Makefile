## handol@gmail.com
CC = g++
INCS = -I$(ACE_ROOT)
CFLAGS = $(DEFS) $(INCS)

MYLIB = libMylib.a
LIBOBJ = MemSplit.o simpleAlloc.o StrUtil.o StrCmp.o URLParse.o FileUtil.o \
 HashUtil.o StrStream.o TimeUtil.o CLog.o Config.o Spliter.o CGI.o \
 cronTable.o

LIBSRC = $(LIBOBJ:.o=.cpp)

all: $(MYLIB) URLParse testSimpleAlloc 

$(LIBOBJ): $(LIBSRC)
	$(CC) -c -Wall $*.cpp -g $(CFLAGS)

$(MYLIB): $(LIBOBJ)
	ar rcv $@ $(LIBOBJ)
	ranlib $@

clean:
	/bin/rm *.o $(MYLIB)
	
#---- test 실행 화일들
testSimpleAlloc: simpleAlloc.cpp
	$(CC) -o $@ -DTEST_MAIN $<

URLParse: URLParse.cpp
	$(CC) -o $@ -DTEST $< -g
	

