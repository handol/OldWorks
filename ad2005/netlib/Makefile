CC=g++

DEFS= -D_REENTRANT -D_GNU_SOURCE
INC= -I./ -I../
#OPT= -pipe -O3 -g
OPT= -O1 -g
WARN_OPTS= -Wall

CFLAGS=$(WARN_OPTS) $(OPT) $(INC) $(DEFS)

EXE=testuseCurl
OBJ = useCurl.o testuseCurl.o
SRC= $(OBJ:.o=.cpp)
MYLIB = 

LIBS= -L../extlib $(MYLIB)  -lcurl -lpthread

all: $(EXE)

$(EXE): $(OBJ) $(MYLIB)
	$(CC) -o $@ $(OBJ) $(LIBS)


.cpp.o: $(SRC)
	$(CC) $(CFLAGS) -c $*.cpp -o $*.o


clean:
	/bin/rm -f *.o $(EXE)

