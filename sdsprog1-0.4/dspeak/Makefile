CC=gcc
CXX=g++
CPPFLAGS=-Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#CPPFLAGS=-Wall -D_GNU_SOURCE -g
#LDFLAGS=-g -L$(LIB_DIR) -L/usr/lib/termcap
LDFLAGS=-g -L/usr/lib/termcap
LOADLIBES= -lm -lfl -lsd -lfn -lapprox -lg2c -lreadline -ltermcap -lstdc++
FC=g77

YACC=bison
YFLAGS=-d -y -pdspeak_yy
LEX=flex
LFLAGS= -Pdspeak_yy

INCLUDE_DIR=/usr/local/include/sd

CXXSOURCES=dspeak.cc

CXXOBJECTS=$(CXXSOURCES:.cc=.o)

YLSOURCES= dspeaklex.l dspeakyacc.y
YLOBJECTS= dspeaklex.o dspeakyacc.o

SOURCES= $(CXXSOURCES) $(YLSOURCES)
OBJECTS= $(CXXOBJECTS) $(YLOBJECTS)

PROGRAMS=dspeak

all: $(PROGRAMS)

clean:
	rm -f *.o *.d *~ $(PROGRAMS) dspeakyacc.c dspeaklex.c y.tab.h

dspeaklex.c: dspeaklex.l y.tab.h

y.tab.h: dspeakyacc.c

dspeakyacc.o: dspeakyacc.c
	gcc -c $(CPPFLAGS) $(CFLAGS) -DYYDEBUG dspeakyacc.c 

%.d: %.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

TAGS: $(INCLUDES) $(YLSOURCES) $(CXXSOURCES)
	etags -t $^

dspeak: $(OBJECTS) $(LIBRARY)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

checkrcs:
	rcsdiff -q $(CXXSOURCES)

install: $(PROGRAMS)
	install -s -m 755 -o 0 -g 0 $(PROGRAMS) /usr/local/bin

tmp-install: $(PROGRAMS)
	cp $(PROGRAMS) $(HOME)/bin

include $(CXXSOURCES:.cc=.d)

