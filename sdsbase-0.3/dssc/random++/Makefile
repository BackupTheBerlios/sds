CC=gcc
CXX=g++
CPPFLAGS=-Wall -g -I./
RANLIB=ranlib

CXXSOURCES=ACG.cc RNG.cc Random.cc Normal.cc

CXXOBJECTS=$(CXXSOURCES:.cc=.o)

INCLUDES=./*.h

SOURCES=$(CXXSOURCES)

OBJECTS=$(CXXOBJECTS)


librandom++.a: $(OBJECTS)
	$(RM) -f $@
	$(AR) rv $@ $^
	$(RANLIB) $@

clean:
	$(RM) *.o  *~ librandom++.a

mostlyclean:
	$(RM) *.o  *~

