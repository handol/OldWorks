## handol@gmail.com
CC = g++ 
DEFS = -DNO_MSSQL -DHDEBUG -DDEB
MYSQL_FLAG = `mysql_config --cflags`
MYSQL_LIB = `mysql_config --libs`

INCS = -I../lib -I../dblib -I../netlib -I../extract -I../hangul -I$(ACE_ROOT) $(MYSQL_FLAG)
CFLAGS = -g -Wall $(DEFS) $(INCS) -DDEB
#OTHER_OBJ = ../hangul/HanChar.o
LFLAGS = -g -L../lib -lMylib -L../dblib -lDblib -L../hangul -lHangullib -lcurl -L$(ACE_ROOT)/lib -lACE $(OTHER_OBJ) $(MYSQL_LIB)

EXE = extractServer

BASE_OBJ = EDict.o TextLex.o TextProc.o
OBJ =  $(BASE_OBJ) extractCron.o ExtractHandler.o ExtractServer.o Extracter.o
SRC = $(OBJ:.o=.cpp)

EXE2 = searchServer
OBJ2 =  $(BASE_OBJ) Docs.o Exam.o Search.o  SearchHandler.o SearchServer.o HExam.o
SRC2 = $(OBJ2:.o=.cpp)

EXE3 = testExtract
OBJ3 = $(BASE_OBJ) extract.o Search.o Docs.o Exam.o HExam.o
SRC3 = $(OBJ3:.o=.cpp)

all: $(EXE) $(EXE2) $(EXE3)

#$(OBJ): $(SRC)
.cpp.o:
	$(CC) $(CFLAGS) -c $*.cpp

$(EXE): $(OBJ)
	$(CC) -o $@ $(OBJ) $(EXT) $(LFLAGS)

$(EXE2): $(OBJ2)
	$(CC) -o $@ $(OBJ2) $(EXT2) $(LFLAGS)

$(EXE3): $(OBJ3)
	$(CC) -o $@ $(OBJ3) $(EXT3) $(LFLAGS)


clean:
	/bin/rm *.o $(EXE)
	

