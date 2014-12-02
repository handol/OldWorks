## handol@gmail.com
CC = g++ 
DEFS = -DNO_MSSQL -DHDEBUG -DDEB
MYSQL_FLAG = `mysql_config --cflags`
MYSQL_LIB = `mysql_config --libs`

INCS = -I../lib -I../dblib -I../netlib -I../harvest -I../hangul -I../extract -I$(ACE_ROOT) $(MYSQL_FLAG)
CFLAGS = -g -Wall $(DEFS) $(INCS) -DDEB
#OTHER_OBJ = ../hangul/HanChar.o
LFLAGS = -g -L../lib -lMylib -L../dblib -lDblib -L../hangul -lHangullib -lcurl -L$(ACE_ROOT)/lib -lACE $(OTHER_OBJ) $(MYSQL_LIB)

EXE = HarvestServer
EXE2 = webfetch

BASE_OBJ = htmlLex.o htmlParse.o htmlTagEntity.o TravExpr.o htmlLinks.o URLTrav.o \
		    ctgList.o categoryMapper.o
EXT = ../netlib/useCurl.o

OBJ =  $(BASE_OBJ) HarvestServer.o  HarvestHandler.o harvestCron.o
SRC = $(OBJ:.o=.cpp)

OBJ2 =  $(BASE_OBJ) webfetch.o 
SRC2 = $(OBJ:.o=.cpp)


all: $(EXE) $(EXE2) $(EXE3)

#$(OBJ): $(SRC)
.cpp.o:
	$(CC) $(CFLAGS) -c $*.cpp

$(EXE): $(OBJ)
	$(CC) -o $@ $(OBJ) $(EXT) $(LFLAGS)

$(EXE2): $(OBJ2)
	$(CC) -o $@ $(OBJ2) $(EXT) $(LFLAGS)

URLParse: URLParse.cpp
	$(CC) -o $@ -DTEST $< -g

clean:
	/bin/rm *.o $(EXE)
	

