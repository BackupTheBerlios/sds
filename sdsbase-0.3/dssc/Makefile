CC=gcc
CXX=g++
CPPFLAGS= -Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR) -Irandom++
#LDFLAGS= -g
LDFLAGS= -g -L$(LIB_DIR) -Lrandom++
LOADLIBES= -lm -lfl -lsd -lrandom++ -lfn -lapprox -lg2c -lstdc++

# problem with bison V1.75
#YACC=bison
YACC=yacc
#YFLAGS=-d -y -pdssc_yy
YFLAGS=-d -pdssc_yy
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


all: librandom++ dssc

librandom++:
	$(MAKE) -C random++ librandom++.a

clean: 
	$(RM) *.o *.d *~ dssclex.c y.tab.h dsscyacc.c dssc
	$(MAKE) -C random++ clean

mostlyclean: 
	$(RM) *.o *~ dssclex.c y.tab.h dsscyacc.c
	$(MAKE) -C random++ mostlyclean

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
