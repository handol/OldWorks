TARGET_BROWSER=KUN

CC=/opt/SUNWspro/bin/CC 

#   Options to be passed to your C++ compiler:
CCOPTIONS=  -mt -compat=4 -g
CCOPTIONS=  -g -compat=4
LLOPTIONS= -mt -compat=4 
LLOPTIONS=


LIBS = $(LLOPTIONS) -lsocket -lnsl 
#-ldl -lrwtool -lpthread -lposix4 -lm

### **   Compile flags:
THREAD_DEFS = -D_REENTERANT -D_POSIX_PTHREAD_SEMANTICS -D_PTHREAD
BASIC_DEFS = -D$(TARGET_BROWSER) -DNO_DEBUG -Dltk 
DEBUG_DEFS = -DSEE_NEWSTAT -DDEBUG_NEWSTAT
DEBUG_DEFS = 
FUNC_DEFS = -DACE_LOG -DCHECK_L4_IP
FUNC_DEFS = -DCHECK_L4_IP

DEFS = $(BASIC_DEFS) $(THREAD_DEFS) $(DEBUG_DEFS) $(FUNC_DEFS)
DEFS += -DDEBUG

CCFLAGS = $(CCOPTIONS) $(TFLAGS) $(DEFS)

OPT= -O1 -g
OPT=

WARN_OPTS= -W -Wall -Wpointer-arith
WARN_OPTS= -Wall

CFLAGS=$(WARN_OPTS) $(OPT) $(INC) $(DEFS)
CFLAGS=$(CCFLAGS)

OBJ1 = HTTPAgent.cpp TCPSocket.cpp LogPushAgent.cpp UDRChecker.cpp HTTPParser.cpp TCPSvr.cpp FileInfo.cpp LogmAgent.cpp LOGSearcher.cpp TimerEx.cpp

OBJ2 = 

EXE = LogmAgent

OBJ = $(OBJ1) $(OBJ2)
SRC= $(OBJ:.o=.cpp)
SRC2= $(OBJ2:.o=.cpp)
HEADER2= $(OBJ2:.o=.h)
DATE= `date +%m%d`

all: rm_new $(EXE)

rm_new:
	/bin/rm -f newSTATErrFilter.o

$(EXE): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIBS)

.SUFFIXES: .pc .c .cpp .o .typ .h


.cpp.o: $(SRC)
	$(CC) $(CFLAGS) -c $*.cpp -o $*.o

	
clean:
	/bin/rm -rf *.o $(EXE) SunWS_cache/*
	CCadmin -clean

install:
	/bin/cp -p $(EXE) /nfs2/download/$(EXE).$(DATE)

tar:
	/bin/tar cf $(EXE).$(DATE).src.tar *.cpp *.h Makefile *.cfg *.txt *.py
	/bin/cp -p $(EXE).$(DATE).src.tar /usr/local/apache2/htdocs
