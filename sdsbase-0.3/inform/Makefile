CC=gcc
CXX=g++
CXXFLAGS=-Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#LDFLAGS=-g
LDFLAGS=-g -L$(LIB_DIR)
LOADLIBES=-lm -lfl -lsd

INCLUDE_DIR=../include
#LIB_DIR=
LIB_DIR=../lib

PROGRAMS=dsstat dsminmax

all: $(PROGRAMS)

clean:
	rm -f *.o *~ $(PROGRAMS)

tmp-install: $(PROGRAMS)
	cp $(PROGRAMS) $(HOME)/bin

install: $(PROGRAMS)
	install -s -m 755 -o 0 -g 0 $(PROGRAMS) /usr/local/bin
