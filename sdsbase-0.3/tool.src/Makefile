CC=gcc
CXX=g++
CXXFLAGS=-Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#LDFLAGS=-g
LDFLAGS=-g -L$(LIB_DIR)
LOADLIBES=-lm -lsd

INCLUDE_DIR=../include
#LIB_DIR=
LIB_DIR=../lib

PROGRAMS=dssp\
	 dsls\
	 dsf\
	 dsrm\
	 dsd_s\
	 dsg\
	 dscat\
	 dsmatch\
	 dslog\
	 dshist\
	 colxy

all: $(PROGRAMS)

clean:
	rm -f *.o *~ $(PROGRAMS)

tmp-install: all
	cp $(PROGRAMS) $(HOME)/bin

install: $(PROGRAMS)
	install -s -m 755 -o 0 -g 0 $(PROGRAMS) /usr/local/bin
	(cd /usr/local/bin; rm -f dsd dss; ln -s dsd_s dsd; ln -s dsd_s dss;)
