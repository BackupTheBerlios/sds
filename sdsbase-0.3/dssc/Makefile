CC=gcc
CXX=g++
CPPFLAGS= -Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#LDFLAGS= -g
LDFLAGS= -g -L$(LIB_DIR)
LOADLIBES= -lm -lfl -lsd -lfn -lapprox -lg2c -lg++

YACC=bison
YFLAGS=-d -y -pdssc_yy
LEX=flex
LFLAGS= -Pdssc_yy

INCLUDE_DIR=../include
#LIB_DIR=
LIB_DIR=../lib

CXXSOURCES=dssc.cc

CXXOBJECTS=$(CXXSOURCES:.cc=.o)

YLSOURCES=dssclex.l dsscyacc.y
YLOBJECTS=dssclex.o dsscyacc.o

INCLUDES=dssc.h $(INCLUDE_DIR)/shareddata.h

SOURCES=$(CXXSOURCES) $(YLSOURCES)
OBJECTS=$(CXXOBJECTS) $(YLOBJECTS)


all: dssc

clean: 
	$(RM) *.o *.d *~ dssclex.c y.tab.h dsscyacc.c dssc

mostlyclean: 
	$(RM) *.o *~ dssclex.c y.tab.h dsscyacc.c

tmp-install: dssc
	cp dssc $(HOME)/bin

install:
	install -s -m 755 -o 0 -g 0 dssc /usr/local/bin

dssclex.c: dssclex.l y.tab.h

y.tab.h: dsscyacc.c

dsscyacc.o: dsscyacc.c
	gcc -c $(CPPFLAGS) $(CFLAGS) -DYYDEBUG dsscyacc.c 

%.d: %.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

TAGS: $(INCLUDES) $(YLSOURCES) $(CXXSOURCES)
	etags -t $^

dssc: $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

include $(CXXSOURCES:.cc=.d)
