CC=gcc
CXX=g++
CXXFLAGS=-Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#CXXFLAGS=-Wall -D_GNU_SOURCE -g
#LDFLAGS=-g -L$(LIB_DIR)
LDFLAGS=-g
LOADLIBES=-lm -lfl -lsd

INCLUDE_DIR=/usr/local/include/sd

PROGRAMS=dsadd
RCSFILES=$(PROGRAMS:%=%.cc)

all: $(PROGRAMS)

clean:
	rm -f *.o *~ $(PROGRAMS)

checkrcs:
	rcsdiff -q $(RCSFILES)

tmp-install: $(PROGRAMS)
	cp $(PROGRAMS) $(HOME)/bin

install: $(PROGRAMS)
	install -s -m 755 -o 0 -g 0 $(PROGRAMS) /usr/local/bin

