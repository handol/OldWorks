LIBS = -lsocket -lnsl
INCLUDE = 
CFLAGS = -g

EXEC = dnslookup
FILES = dnslookup.cpp

$(EXEC): ${FILES}
	CC $(CFLAGS) $(INCLUDE) $(LIBS) $(FILES) -o $@

clean:
	rm -f *.o $(EXEC)
