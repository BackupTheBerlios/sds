CC=gcc
CXX=g++
CXXFLAGS=-Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#LDFLAGS=-g
LDFLAGS=-g -L$(LIB_DIR)
LOADLIBES=-lm -lfl -lsd -lstdc++

INCLUDE_DIR=../include
#LIB_DIR=
LIB_DIR=../lib

YACC=bison
YFLAGS= -d -y -pdsh_yy
LEX=flex
LFLAGS= -Pdsh_yy

CXXSOURCES=dslold.cc

CXXOBJECTS=$(CXXSOURCES:.cc=.o)

YLSOURCES=dshlex.l dshyacc.y

YLOBJECTS=dshlex.o dshyacc.o

SOURCES=$(CXXSOURCES) $(YLSOURCES) 

OBJECTS=$(CXXOBJECTS) $(YLOBJECTS)

all: dslold

clean:
	$(RM) *.o *.d *~ dshyacc.c dshlex.c y.tab.h dslold

tmp-install: all
	cp dslold $(HOME)/bin

dshlex.c:  dshlex.l y.tab.h

dshyacc.o: dshyacc.c
	gcc -c $(CPPFLAGS) $(CFLAGS) -DYYDEBUG dshyacc.c 

y.tab.h: dshyacc.c

TAGS: $(INCLUDES) $(YLSOURCES) $(CXXSOURCES)
	etags -t $^

install: all
	install -s -m 755 -o 0 -g 0 dslold /usr/local/bin

%.d: %.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

dslold: $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) -o $@
