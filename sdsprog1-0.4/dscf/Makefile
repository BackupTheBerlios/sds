CC=gcc
CXX=g++
FC=g77
CPPFLAGS=-Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#CPPFLAGS=-Wall -D_GNU_SOURCE -g
#LDFLAGS=-g -L$(LIB_DIR) -L/usr/lib/termcap
LDFLAGS=-g -L/usr/lib/termcap
LOADLIBES=-lm -lfl -lsd -lreadline -ltermcap -lg++ -lg2c

YACC=bison
YFLAGS=-d -y -pdscfl_yy
LEX=flex
LFLAGS= -Pdscfl_yy

INCLUDE_DIR=/usr/local/include/sd

CXXSOURCES=dscf.cc
FSOURCES=blas.f tn.f

CXXOBJECTS=$(CXXSOURCES:.cc=.o)
FOBJECTS=$(FSOURCES:.f=.o)

YLSOURCES= dscflex.l dscfyacc.y
YLOBJECTS= dscflex.o dscfyacc.o

SOURCES= $(CXXSOURCES) $(FSOURCES) $(YLSOURCES)
OBJECTS= $(CXXOBJECTS) $(FOBJECTS) $(YLOBJECTS)

PROGRAMS=dscf

all: $(PROGRAMS)

clean:
	rm -f *.o *.d *~ $(PROGRAMS) dscfyacc.c dscflex.c y.tab.h

dscflex.c: dscflex.l y.tab.h

y.tab.h: dscfyacc.c

dscfyacc.o: dscfyacc.c
	gcc -c $(CPPFLAGS) $(CFLAGS) -DYYDEBUG dscfyacc.c 

%.d: %.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

TAGS: $(INCLUDES) $(YLSOURCES) $(CXXSOURCES)
	etags -t $^

dscf: $(OBJECTS) $(LIBRARY)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

checkrcs:
	rcsdiff -q $(CXXSOURCES)

install: $(PROGRAMS)
	install -s -m 755 -o 0 -g 0 $(PROGRAMS) /usr/local/bin

tmp-install: $(PROGRAMS)
	cp  $(PROGRAMS) $(HOME)/bin

include $(CXXSOURCES:.cc=.d)
