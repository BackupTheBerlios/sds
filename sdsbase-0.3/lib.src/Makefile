CC=gcc
CXX=g++
CPPFLAGS= -fPIC -Wall -DDEBUG -D_GNU_SOURCE -g -I../include
RANLIB=ranlib

CXXSOURCES=	sdpublic.cc\
		sdprivate.cc\
		sdmalloc.cc\
		sdtool.cc\
		color.cc\
		linestyle.cc\
		symbolstyle.cc

CXXOBJECTS=$(CXXSOURCES:.cc=.o)

INCLUDES=./include/*.h

SOURCES=$(CXXSOURCES)

OBJECTS=$(CXXOBJECTS)

LLIB=libsd.so
SDVER=0
SDMINOR=3
LIBSO=$(LLIB).$(SDVER)
LIB=$(LIBSO).$(SDMINOR)

# build shared library: lifted from the Program-Library-HOWTO
# -lc probably not necessary.
$(LIB): $(OBJECTS)
	gcc -g -shared -Wl,-soname,$(LIBSO) -o $@ $^ -lc

libsd.a: $(OBJECTS)
	$(RM) -f $@
	$(AR) rv $@ $^
	$(RANLIB) $@

clean:
	$(RM) *.o *.d *~ dshyacc.c libsd.a $(LIB) tesd

TAGS: $(INCLUDES) $(CXXSOURCES)
	etags -t $^

tmp-install: $(LIB)
	cp $(LIB) ../lib
	(cd ../lib ; ln -sf $(LIB) $(LIBSO) ; \
	             ln -sf $(LIBSO) $(LLIB))

install: $(LIB)
	install -m 755 -o 0 -g 0 $< /usr/local/lib
	ldconfig -n /usr/local/lib
	(cd /usr/local/lib ; ln -sf $(LIBSO) $(LLIB))

%.d: %.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

tesd: tesd.cc libsd.a
	gcc -Wall -g -o tesd tesd.cc libsd.a -I../include

include $(CXXSOURCES:.cc=.d)

