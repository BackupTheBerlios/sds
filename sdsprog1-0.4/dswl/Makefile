CC=gcc
CXX=g++
CPPFLAGS=-Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#CPPFLAGS=-Wall -D_GNU_SOURCE -g
#LDFLAGS=-g -L$(LIB_DIR) -L/usr/lib/termcap
LDFLAGS=-g -L/usr/lib/termcap
LOADLIBES= -lm -lfl -lsd -lfn -lapprox -lg2c -lreadline -ltermcap
FC=g77

YACC=bison
YFLAGS=-d -y -pdswl_yy
LEX=flex
LFLAGS= -Pdswl_yy

INCLUDE_DIR=/usr/local/include/sd

CXXSOURCES=dswl.cc

CXXOBJECTS=$(CXXSOURCES:.cc=.o)

YLSOURCES= dswllex.l dswlyacc.y
YLOBJECTS= dswllex.o dswlyacc.o

SOURCES= $(CXXSOURCES) $(YLSOURCES)
OBJECTS= $(CXXOBJECTS) $(YLOBJECTS)

PROGRAMS=dswl

all: $(PROGRAMS)

clean:
	rm -f *.o *.d *~ $(PROGRAMS) dswlyacc.c dswllex.c y.tab.h

dswllex.c: dswllex.l y.tab.h

y.tab.h: dswlyacc.c

dswlyacc.o: dswlyacc.c
	gcc -c $(CPPFLAGS) $(CFLAGS) -DYYDEBUG dswlyacc.c 

%.d: %.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

TAGS: $(INCLUDES) $(YLSOURCES) $(CXXSOURCES)
	etags -t $^

dswl: $(OBJECTS) $(LIBRARY)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

checkrcs:
	rcsdiff -q $(CXXSOURCES)

install: $(PROGRAMS)
	install -s -m 755 -o 0 -g 0 $(PROGRAMS) /usr/local/bin

tmp-install: $(PROGRAMS)
	cp $(PROGRAMS) $(HOME)/bin

include $(CXXSOURCES:.cc=.d)

