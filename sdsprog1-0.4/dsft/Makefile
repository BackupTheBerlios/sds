CC=gcc
CXX=g++
CPPFLAGS=-Wall -D_GNU_SOURCE -g -I$(INCLUDE_DIR)
#CPPFLAGS=-Wall -D_GNU_SOURCE -g
#LDFLAGS=-g -L$(LIB_DIR) -L/usr/lib/termcap
LDFLAGS=-g 
#LDFLAGS=-g
LOADLIBES=-lm -lsd

INCLUDE_DIR=/usr/local/include/sd

CXXSOURCES=dsft.cc

CXXOBJECTS=$(CXXSOURCES:.cc=.o)

SOURCES= $(CXXSOURCES)
OBJECTS= $(CXXOBJECTS)

PROGRAMS=dsft

all: $(PROGRAMS)

clean:
	rm -f *.o *.d *~ $(PROGRAMS)

%.d: %.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

TAGS: $(INCLUDES) $(CXXSOURCES)
	etags -t $^

checkrcs:
	rcsdiff -q $(CXXSOURCES)

tmp-install: $(PROGRAMS)
	cp $(PROGRAMS) $(HOME)/bin

install: $(PROGRAMS)
	install -s -m 755 -o 0 -g 0 $(PROGRAMS) /usr/local/bin

include $(CXXSOURCES:.cc=.d)

time_duhamel: time_duhamel.c fft_duhamel.c
	cc -O4 -o time_duhamel  time_duhamel.c fft_duhamel.c -lm

time_mayer: time_mayer.c fft_mayer.c
	cc -O4 -o time_mayer  time_mayer.c fft_mayer.c

time_r_mayer: time_r_mayer.c fft_mayer.c
	cc -O4 -o time_r_mayer  time_r_mayer.c fft_mayer.c

test_duhamel: test_duhamel.c fft_duhamel.c
	cc -O4 -o test_duhamel  test_duhamel.c fft_duhamel.c -lm

test_mayer: test_mayer.c fft_mayer.c
	cc -O4 -o test_mayer  test_mayer.c fft_mayer.c

