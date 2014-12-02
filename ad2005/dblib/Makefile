## handol@gmail.com
CC = g++

MYSQL_FLAG = `mysql_config --cflags`
MYSQL_LIB = `mysql_config --libs`
CCFLAGS =  -DNO_MSSQL -I../lib -I../harvest -I../extract -I../hangul $(MYSQL_FLAG)
MYLIB = libDblib.a
LIBOBJ = dbStore.o DbConn_mysql.o DbQuery_mysql.o
LD_FLAG = $(MYSQL_LIB)
LIBSRC = $(LIBOBJ:.o=.cpp)

all: $(MYLIB)

$(LIBOBJ): $(LIBSRC)
	$(CC) -c -Wall $*.cpp -g $(CCFLAGS)

$(MYLIB): $(LIBOBJ)
	ar rcv $@ $(LIBOBJ)
	ranlib $@

clean:
	/bin/rm *.o $(MYLIB)
	

