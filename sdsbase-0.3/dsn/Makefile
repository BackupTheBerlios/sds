CC=gcc
CXX=g++
CPPFLAGS= -Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#LDFLAGS= -g
LDFLAGS= -g -L$(LIB_DIR)
LOADLIBES= -lm -lfl -lsd -lstdc++

YACC= bison 
YFLAGS= -d -y -pdsn_yy
LEX= flex
LFLAGS= -Pdsn_yy

INCLUDE_DIR=../include
#LIB_DIR=
LIB_DIR=../lib


CXXSOURCES=dsn.cc

CXXOBJECTS=$(CXXSOURCES:.cc=.o)

YLSOURCES=dsnlex.l dsnyacc.y
YLOBJECTS=dsnlex.o dsnyacc.o

INCLUDES=dssc.h $(INCLUDE_DIR)/shareddata.h

SOURCES=$(CXXSOURCES) $(YLSOURCES)
OBJECTS=$(CXXOBJECTS) $(YLOBJECTS)


all: dsn

clean: 
	$(RM) *.o *.d *~ dsnlex.c y.tab.h dsnyacc.c dsn

mostlyclean: 
	$(RM) *.o *~ dsnlex.c y.tab.h dsnyacc.c

tmp-install: dsn
	cp dsn $(HOME)/bin

install:
	install -s -m 755 -o 0 -g 0 dsn /usr/local/bin

dsnlex.c: dsnlex.l y.tab.h

y.tab.h: dsnyacc.c

dsnyacc.o: dsnyacc.c
	gcc -c $(CPPFLAGS) $(CFLAGS) -DYYDEBUG dsnyacc.c 

%.d: %.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

TAGS: $(INCLUDES) $(YLSOURCES) $(CXXSOURCES)
	etags -t $^

dsn: $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

include $(CXXSOURCES:.cc=.d)

