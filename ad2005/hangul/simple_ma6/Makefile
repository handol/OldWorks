#############################################################################
# Makefile for building jangi 
# Generated by tmake at 12:34, 2003/11/28
#     Project: jangi 
#    Template: app
#############################################################################

####### Compiler, tools and options

CC	=	gcc
CC	=	g++
CXX	=	g++
# changed for using gnome_dialog at step 4 
CFLAGS  =    

HEADERS =	
SOURCES =        \
		sentence_ana.c \
		ma.c \
		hangul.c \
		hanja.c \
		number.c \
		ttma.c \
		main.c 
OBJECTS =       \
		sentence_ana.o \
		ma.o \
		hangul.o \
		hanja.o \
		number.o \
		ttma.o \
		main.o 
INTERFACES =	
UICDECLS =	
UICIMPLS =	
SRCMOC	=	
OBJMOC	=	
DIST	=	
TARGET	= simple_ma	
INTERFACE_DECL_PATH = .
LINK =gcc
LINK =g++
LFLAGS = -lm 
####### Implicit rules

.SUFFIXES: .cpp .cxx .cc .C .c

.cpp.o:
	$(CXX) -g -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -g -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -g -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	$(CXX) -g -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -g -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules


all: $(TARGET)

$(TARGET): $(UICDECLS) $(OBJECTS) $(OBJMOC) 
	$(LINK) $(LFLAGS) -g -o $(TARGET) $(OBJECTS) $(OBJMOC) $(LIBS)

moc: $(SRCMOC)

tmake: Makefile


dist:
	$(TAR) gostop.tar gostop.pro $(SOURCES) $(HEADERS) $(INTERFACES) $(DIST)
	$(GZIP) gostop.tar

clean:
	-rm -f $(OBJECTS) $(OBJMOC) $(SRCMOC) $(UICIMPLS) $(UICDECLS) $(TARGET)
	-rm -f *~ core

####### Sub-libraries


###### Combined headers


####### Compile

gui.o: gui.c

main.o: main.c

