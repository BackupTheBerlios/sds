CC=gcc
CXX=g++
CPPFLAGS= -Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#LDFLAGS= -g
LDFLAGS= -g -L$(LIB_DIR)
LOADLIBES= -lm -lfl -lsd -lstdc++

YACC= bison 
YFLAGS= -d -y 
LEX= flex
LFLAGS=

INCLUDE_DIR=../include
#LIB_DIR=
LIB_DIR=../lib

CSOURCES=
CXXSOURCES=dsl.cc

COBJECTS=$(CSOURCES:.c=.o)
CXXOBJECTS=$(CXXSOURCES:.cc=.o)

YLSOURCES=dshlex.l dshyacc.y datlex.l datyacc.y
YLOBJECTS=dshlex.o dshyacc.o datlex.o datyacc.o

SOURCES=$(CXXSOURCES) $(CSOURCES) $(YLSOURCES)
OBJECTS=$(CXXOBJECTS) $(COBJECTS) $(YLOBJECTS)


all: dsl

clean: 
	$(RM) *.o *.d *~ dshlex.c dshyacc.h dshyacc.c datlex.c datyacc.h \
	 datyacc.c dsl

mostlyclean: 
	$(RM) *.o *~ dshlex.c y.tab.h dshyacc.c

tmp-install: dsl
	cp dsl $(HOME)/bin

install:
	install -s -m 755 -o 0 -g 0 dsl /usr/local/bin

dshyacc.h: dshyacc.c

dshyacc.c: dshyacc.y
	$(YACC) $(YFLAGS) -o $@ -pdsh_yy $<

dshyacc.o: dshyacc.c
	g++ -c $(CPPFLAGS) $(CFLAGS) -DYYDEBUG dshyacc.c 

dshlex.c: dshlex.l dshyacc.h
	$(LEX) $(LFLAGS) -t -Pdsh_yy $< > $@

dshlex.o: dshlex.c
	g++ -c $(CPPFLAGS) $(CFLAGS) dshlex.c 

datyacc.h: datyacc.c

datyacc.c: datyacc.y
	$(YACC) $(YFLAGS) -o $@ -pdat_yy $<

datyacc.o: datyacc.c
	g++ -c $(CPPFLAGS) $(CFLAGS) -DYYDEBUG datyacc.c 

datlex.c: datlex.l datyacc.h
	$(LEX) $(LFLAGS) -t -Pdat_yy $< > $@

datlex.o: datlex.c
	g++ -c $(CPPFLAGS) $(CFLAGS) datlex.c 

$(CXXSOURCES:.cc=.d): %.d: %.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

#$(CSOURCES:.c=.d): %.d: %.c
#	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
#	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
#	[ -s $@ ] || rm -f $@'

TAGS: $(INCLUDES) $(YLSOURCES) $(CXXSOURCES)
	etags -t $^

dsl: $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

include $(CXXSOURCES:.cc=.d)

#include $(CSOURCES:.c=.d)

