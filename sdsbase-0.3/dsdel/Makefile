CC=gcc
CXX=g++
CPPFLAGS= -Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#LDFLAGS= -g 
LDFLAGS= -g -L$(LIB_DIR)
LOADLIBES= -lm -lfl -lsd

YACC= bison 
YFLAGS= -d -y -pdsdel_yy
LEX= flex
LFLAGS= -Pdsdel_yy

INCLUDE_DIR=../include
LIB_DIR=../lib
#LIB_DIR=


CXXSOURCES=dsdel.cc

CXXOBJECTS=$(CXXSOURCES:.cc=.o)

YLSOURCES=dsdellex.l dsdelyacc.y
YLOBJECTS=dsdellex.o dsdelyacc.o

INCLUDES=dssc.h $(INCLUDE_DIR)/shareddata.h

SOURCES=$(CXXSOURCES) $(YLSOURCES)
OBJECTS=$(CXXOBJECTS) $(YLOBJECTS)


all: dsdel

clean: 
	$(RM) *.o *.d *~ dsdellex.c y.tab.h dsdelyacc.c dsdel

mostlyclean: 
	$(RM) *.o *~ dsdellex.c y.tab.h dsdelyacc.c

tmp-install: dsdel
	cp dsdel $(HOME)/bin

install:
	install -s -m 755 -o 0 -g 0 dsdel /usr/local/bin

dsdellex.c: dsdellex.l y.tab.h

y.tab.h: dsdelyacc.c

dsdelyacc.o: dsdelyacc.c
	gcc -c $(CPPFLAGS) $(CFLAGS) -DYYDEBUG dsdelyacc.c 

%.d: %.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

TAGS: $(INCLUDES) $(YLSOURCES) $(CXXSOURCES)
	etags -t $^

dsdel: $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

include $(CXXSOURCES:.cc=.d)

