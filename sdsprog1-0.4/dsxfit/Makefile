CC=gcc
CXX=g++
CPPFLAGS=-Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#CPPFLAGS=-Wall -D_GNU_SOURCE -g
#LDFLAGS=-g -L$(LIB_DIR) -L/usr/lib/termcap
LDFLAGS=-g -L/usr/lib/termcap
LOADLIBES= -lm -lfl -lstdc++ -lsd -lfn -lapprox -lg2c -lreadline -ltermcap

# problem with bison V1.75
#YACC=bison
YACC=yacc
#YFLAGS=-d -y 
YFLAGS=-d 
LEX=flex
LFLAGS=

INCLUDE_DIR=/usr/local/include/sd

CXXSOURCES=dsxfit.cc

CXXOBJECTS=$(CXXSOURCES:.cc=.o)

YLSOURCES= dsfitlex.l dsfityacc.y dssclex.l dsscyacc.y
YLOBJECTS= dsfitlex.o dsfityacc.o dssclex.o dsscyacc.o

SOURCES= $(CXXSOURCES) $(YLSOURCES)
OBJECTS= $(CXXOBJECTS) $(YLOBJECTS)

PROGRAMS=dsxfit

all: $(PROGRAMS)

clean:
	rm -f *.o *.d *~ $(PROGRAMS) dsfityacc.c dsfitlex.c dsfityacc.h \
	                             dsscyacc.c dssclex.c dsscyacc.h

dsfityacc.h: dsfityacc.c

dsfityacc.c: dsfityacc.y
	$(YACC) $(YFLAGS) -pdsfit_yy $<
	mv y.tab.c $@
	mv y.tab.h dsfityacc.h

dsfityacc.o: dsfityacc.c
	gcc -c $(CPPFLAGS) $(CFLAGS) -DYYDEBUG dsfityacc.c 

dsfitlex.c: dsfitlex.l dsfityacc.h
	$(LEX) $(LFLAGS) -t -Pdsfit_yy $< > $@

dsfitlex.o: dsfitlex.c
	gcc -c $(CPPFLAGS) $(CFLAGS) dsfitlex.c

dsscyacc.h: dsscyacc.c

dsscyacc.c: dsscyacc.y
	$(YACC) $(YFLAGS) -pdssc_yy $<
	mv y.tab.c $@
	mv y.tab.h dsscyacc.h

dsscyacc.o: dsscyacc.c
	gcc -c $(CPPFLAGS) $(CFLAGS) -DYYDEBUG dsscyacc.c 

dssclex.c: dssclex.l dsscyacc.h
	$(LEX) $(LFLAGS) -t -Pdssc_yy $< > $@

dssclex.o: dssclex.c
	gcc -c $(CPPFLAGS) $(CFLAGS) dssclex.c

%.d: %.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

TAGS: $(INCLUDES) $(YLSOURCES) $(CXXSOURCES)
	etags -t $^

dsxfit: $(OBJECTS) $(LIBRARY)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

checkrcs:
	rcsdiff -q $(CXXSOURCES)

tmp-install: $(PROGRAMS)
	cp  $(PROGRAMS) $(HOME)/bin

install: $(PROGRAMS)
	install -s -m 755 -o 0 -g 0 $(PROGRAMS) /usr/local/bin

include $(CXXSOURCES:.cc=.d)
