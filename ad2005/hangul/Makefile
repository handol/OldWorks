## handol@gmail.com
CC = g++ 
DEFS = -DNO_MSSQL
MYSQL_FLAG = `mysql_config --cflags`
MYSQL_LIB = `mysql_config --libs`
INCS = -I../lib -I../dblib -I../netlib -I../extract -I../hangul -I$(ACE_ROOT)
CFLAGS = -g -Wall $(DEFS) $(INCS) $(MYSQL_FLAG)
LFLAGS = -g -L../lib -lMylib -L../dblib -lDblib -L$(ACE_ROOT)/lib -lACE $(MYSQL_LIB)

EXE = hmorph

H_BASE_OBJ = HanChar.o HDict.o HStem.o HGram.o HProc.o
OBJ =  $(H_BASE_OBJ) MorphHandler.o MorphServer.o 
SRC = $(OBJ:.o=.cpp)

EXE2 = htest
OBJ2 =  $(H_BASE_OBJ) hangulMain.o

SRC2 = $(OBJ2:.o=.cpp)

all: $(EXE) $(EXE2)

#$(OBJ): $(SRC)
.cpp.o:
	$(CC) $(CFLAGS) -c $*.cpp

$(EXE): $(OBJ)
	$(CC) -o $@ $(OBJ) $(EXT) $(LFLAGS)

$(EXE2): $(OBJ2)
	$(CC) -o $@ $(OBJ2) $(EXT2) $(LFLAGS)

clean:
	/bin/rm *.o $(EXE)
	

