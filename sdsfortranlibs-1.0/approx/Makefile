FC=g77
FFLAGS= -Wall -g -fPIC

F77SOURCES=$(wildcard *.f)

F77OBJECTS=$(F77SOURCES:.f=.o)

CHEADER= approx.h

SOURCES=$(F77SOURCES)

OBJECTS=$(F77OBJECTS)

LLIB=libapprox.so
LIBVER=1
LIBMINOR=0
LIBSO=$(LLIB).$(LIBVER)
LIB=$(LIBSO).$(LIBMINOR)

# build shared library - from the Program-Library-HOWTO
$(LIB): $(OBJECTS)
	gcc -g -shared -Wl,-soname,$(LIBSO) -o $@ $^

libapprox.a: $(OBJECTS)
	$(RM) -f $@
	$(AR) rv $@ $^
	$(AR) sv $@

cproto: $(F77SOURCES:.f=.P)

%.P : %.f
	f2c -P -C++ $<
	rm $*.c

clean:
	$(RM) *.o *.P *~ $(LIB) 

#TAGS: $(INCLUDES) $(YLSOURCES) $(CXXSOURCES)
#	etags -t $^

tmp-install: $(LIB)
	cp $(LIB) ../lib
	(cd ../lib ; ln -sf $(LIB) $(LIBSO) ; \
	             ln -sf $(LIBSO) $(LLIB))

install: $(LIB)
	install -m 755 -o 0 -g 0 $< /usr/local/lib
	ldconfig -n /usr/local/lib
	(cd /usr/local/lib ; ln -sf $(LIBSO) $(LLIB))

