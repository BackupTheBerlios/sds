CC=gcc
CXX=g++
CPPFLAGS=-Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#CPPFLAGS=-Wall -D_GNU_SOURCE -g
#LDFLAGS=-g -L$(LIB_DIR) -L/usr/lib/termcap
LDFLAGS=-g -L/usr/lib/termcap
LOADLIBES= -lm -lfl -lsd -lfn -lapprox -lg2c -lreadline -ltermcap

YACC=bison
YFLAGS=-d -y -pdsfit_yy
LEX=flex
LFLAGS= -Pdsfit_yy

INCLUDE_DIR=/usr/local/include/sd

CXXSOURCES=dsfit.cc

CXXOBJECTS=$(CXXSOURCES:.cc=.o)

YLSOURCES= dsfitlex.l dsfityacc.y
YLOBJECTS= dsfitlex.o dsfityacc.o

SOURCES= $(CXXSOURCES) $(YLSOURCES)
OBJECTS= $(CXXOBJECTS) $(YLOBJECTS)

PROGRAMS=dsfit

all: $(PROGRAMS)

clean:
	rm -f *.o *.d *~ $(PROGRAMS) dsfityacc.c dsfitlex.c y.tab.h

dsfitlex.c: dsfitlex.l y.tab.h

y.tab.h: dsfityacc.c

dsfityacc.o: dsfityacc.c
	gcc -c $(CPPFLAGS) $(CFLAGS) -DYYDEBUG dsfityacc.c 

%.d: %.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

TAGS: $(INCLUDES) $(YLSOURCES) $(CXXSOURCES)
	etags -t $^

dsfit: $(OBJECTS) $(LIBRARY)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

checkrcs:
	rcsdiff -q $(CXXSOURCES)

tmp-install: $(PROGRAMS)
	cp  $(PROGRAMS) $(HOME)/bin

install: $(PROGRAMS)
	install -s -m 755 -o 0 -g 0 $(PROGRAMS) /usr/local/bin

include $(CXXSOURCES:.cc=.d)
