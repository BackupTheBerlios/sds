CC=gcc
CXX=g++
CPPFLAGS= -Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#LDFLAGS= -g
LDFLAGS= -g -L$(LIB_DIR)
LOADLIBES= -lm -lfl -lsd

YACC= bison 
YFLAGS= -d -y 
LEX= flex
LFLAGS=

INCLUDE_DIR=../include
#LIB_DIR=
LIB_DIR=../lib

CSOURCES=
CXXSOURCES=dsreg.cc util.cc

COBJECTS=$(CSOURCES:.c=.o)
CXXOBJECTS=$(CXXSOURCES:.cc=.o)

YLSOURCES=dsreglex.l dsregyacc.y dshlex.l dshyacc.y
YLOBJECTS=dsreglex.o dsregyacc.o dshlex.o dshyacc.o

SOURCES=$(CXXSOURCES) $(CSOURCES) $(YLSOURCES)
OBJECTS=$(CXXOBJECTS) $(COBJECTS) $(YLOBJECTS)


all: dsreg

clean: 
	$(RM) *.o *.d *~ dsreglex.c dsregyacc.h dsregyacc.c dsreg \
			 dshlex.c dshyacc.h dshyacc.c

mostlyclean: 
	$(RM) *.o *~ dsreglex.c y.tab.h dsregyacc.c

tmp-install: dsreg
	cp dsreg $(HOME)/bin

install:
	install -s -m 755 -o 0 -g 0 dsreg /usr/local/bin

dsregyacc.h: dsregyacc.c

dsregyacc.c: dsregyacc.y
	$(YACC) $(YFLAGS) -o $@ -pdsreg_yy $<

dsregyacc.o: dsregyacc.c
	g++ -c $(CPPFLAGS) $(CFLAGS) -DYYDEBUG dsregyacc.c 

dsreglex.c: dsreglex.l dsregyacc.h
	$(LEX) $(LFLAGS) -t -Pdsreg_yy $< > $@

dsreglex.o: dsreglex.c
	g++ -c $(CPPFLAGS) $(CFLAGS) dsreglex.c 

dshyacc.h: dshyacc.c

dshyacc.c: dshyacc.y
	$(YACC) $(YFLAGS) -o $@ -pdsh_yy $<

dshyacc.o: dshyacc.c
	g++ -c $(CPPFLAGS) $(CFLAGS) -DYYDEBUG dshyacc.c 

dshlex.c: dshlex.l dshyacc.h
	$(LEX) $(LFLAGS) -t -Pdsh_yy $< > $@

dshlex.o: dshlex.c
	g++ -c $(CPPFLAGS) $(CFLAGS) dshlex.c 

$(CXXSOURCES:.cc=.d): %.d: %.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

$(CSOURCES:.c=.d): %.d: %.c
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

TAGS: $(INCLUDES) $(YLSOURCES) $(CXXSOURCES)
	etags -t $^

dsreg: $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

include $(CXXSOURCES:.cc=.d)

# include $(CSOURCES:.c=.d)

