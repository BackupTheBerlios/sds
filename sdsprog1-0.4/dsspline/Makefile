CC=gcc
CXX=g++
CPPFLAGS=-Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#CPPFLAGS=-Wall -D_GNU_SOURCE -g
#LDFLAGS=-g -L$(LIB_DIR) -L/usr/lib/termcap
LDFLAGS=-g -L/usr/lib/termcap 
LOADLIBES=-lm -lfl -lsd -lspline -lreadline -ltermcap -lg2c

YACC=bison
YFLAGS=-d -y -pdsspl_yy
LEX=flex
LFLAGS= -Pdsspl_yy

INCLUDE_DIR=/usr/local/include/sd

CXXSOURCES=dsspline.cc

CXXOBJECTS=$(CXXSOURCES:.cc=.o)

YLSOURCES= dssplex.l dsspyacc.y
YLOBJECTS= dssplex.o dsspyacc.o

SOURCES= $(CXXSOURCES) $(YLSOURCES)
OBJECTS= $(CXXOBJECTS) $(YLOBJECTS)

PROGRAMS=dsspline

all: $(PROGRAMS)

clean:
	rm -f *.o *.d *~ $(PROGRAMS) dsspyacc.c dssplex.c y.tab.h

dssplex.c: dssplex.l y.tab.h

y.tab.h: dsspyacc.c

dsspyacc.o: dsspyacc.c
	gcc -c $(CPPFLAGS) $(CFLAGS) -DYYDEBUG dsspyacc.c 

%.d: %.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

TAGS: $(INCLUDES) $(YLSOURCES) $(CXXSOURCES)
	etags -t $^

dsspline: $(OBJECTS) $(LIBRARY)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

checkrcs:
	rcsdiff -q $(CXXSOURCES)

tmp-install: $(PROGRAMS)
	cp $(PROGRAMS) $(HOME)/bin

install: $(PROGRAMS)
	install -s -m 755 -o 0 -g 0 $(PROGRAMS) /usr/local/bin

include $(CXXSOURCES:.cc=.d)
