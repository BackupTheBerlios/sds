CC=gcc
CXX=g++
CPPFLAGS=-Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#CPPFLAGS=-Wall -D_GNU_SOURCE -g
#LDFLAGS=-g -L$(LIB_DIR) -L/usr/lib/termcap
LDFLAGS=-g -L/usr/lib/termcap 
LOADLIBES=-lm -lfl -lsd -lspline -lreadline -ltermcap -lg2c

YACC=bison
YFLAGS=-d -y -pdsdydx_yy
LEX=flex
LFLAGS= -Pdsdydx_yy

INCLUDE_DIR=/usr/local/include/sd

CXXSOURCES=dsdydx.cc

CXXOBJECTS=$(CXXSOURCES:.cc=.o)

YLSOURCES= dsdydxlex.l dsdydxyacc.y
YLOBJECTS= dsdydxlex.o dsdydxyacc.o

SOURCES= $(CXXSOURCES) $(YLSOURCES)
OBJECTS= $(CXXOBJECTS) $(YLOBJECTS)

PROGRAMS=dsdydx

all: $(PROGRAMS)

clean:
	rm -f *.o *.d *~ $(PROGRAMS) dsdydxyacc.c dsdydxlex.c y.tab.h

dsdydxlex.c: dsdydxlex.l y.tab.h

y.tab.h: dsdydxyacc.c

dsdydxyacc.o: dsdydxyacc.c
	gcc -c $(CPPFLAGS) $(CFLAGS) -DYYDEBUG dsdydxyacc.c 

%.d: %.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

TAGS: $(INCLUDES) $(YLSOURCES) $(CXXSOURCES)
	etags -t $^

dsdydx: $(OBJECTS) $(LIBRARY)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

checkrcs:
	rcsdiff -q $(CXXSOURCES)

tmp-install: $(PROGRAMS)
	cp $(PROGRAMS) $(HOME)/bin

install: $(PROGRAMS)
	install -s -m 755 -o 0 -g 0 $(PROGRAMS) /usr/local/bin

include $(CXXSOURCES:.cc=.d)
