CC=gcc
CXX=g++
CXXFLAGS=-Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#LDFLAGS=-g
LDFLAGS=-g -L$(LIB_DIR)
LOADLIBES=-lm -lfl -lsd

INCLUDE_DIR=../include
#LIB_DIR=
LIB_DIR=../lib

PROGRAMS=dsaco\
	 dseinige\
	 dsfilter\
	 dsfunc\
	 dsint\
	 dssort\
	 dscat\
	 dspad\
	 dsdeglitch

#        dscos ( temporarily, will turn to a fit program)
#	 dsfraktal
#        is not yet converted to the new SharedData strategie
#        (which should be easy, by the way)


all: $(PROGRAMS)

clean:
	rm -f *.o *~ $(PROGRAMS) dscos

tmp-install: $(PROGRAMS)
	cp $(PROGRAMS) $(HOME)/bin

install: $(PROGRAMS)
	install -s -m 755 -o 0 -g 0 $(PROGRAMS) /usr/local/bin
