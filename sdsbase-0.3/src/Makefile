# $Author: vstein $ 
# $Revision: 1.1 $
# $Id: Makefile,v 1.1 2003/01/23 14:50:49 vstein Exp $

CXXFLAGS=-g -Wall -D_GNU_SOURCE -DDEBUG -I$(QTDIR)/include -I../include
#LDFLAGS=-g -Wall -L$(QTDIR)/lib
LDFLAGS=-g -Wall -L$(QTDIR)/lib -L../lib
LOADLIBES=-lqt -lm -lsd


HEADERS =	../include/parea.h \
		../include/SharedData.h

SOURCES =	parea.cc \
		qpl.cc

OBJECTS =	parea.o \
		qpl.o

SRCMOC  =	moc_parea.cc \
		moc_qpl.cc\
		moc_qpl1.cc

QMOBJS =	qm.o

QMHEADERS =	../include/sds/shareddata.h\
		../include/sds/color.h\
		../include/sds/linestyle.h\
		../include/sds/symbolstyle.h

ifeq ($(QTDIR),/usr/lib/qt2)
  OBJMOC  =	moc_parea.o moc_qpl.o
  MOC	  =	/usr/lib/qt2/bin/moc
  CXXFLAGS +=	-DSETITEMPARAM
else 
  ifeq ($(QTDIR),/usr/lib/qt3)
    OBJMOC = moc_parea.o moc_qpl.o
    MOC    = /usr/lib/qt3/bin/moc
    CXXFLAGS +=	-DSETITEMPARAM
  else
    OBJMOC =	moc_parea.o moc_qpl1.o
    MOC	   =	moc
  endif
endif

#.cc.o:
#	$(CXX) -c $(CXXFLAGS) -o $@ $<

all: qpl

qpl: $(OBJECTS) $(OBJMOC) $(HEADERS)
	g++ $(LDFLAGS) -o qpl $(OBJECTS) $(OBJMOC) $(LOADLIBES)

moc_parea.o:	moc_parea.cc \
		../include/parea.h

moc_parea.cc: 	../include/parea.h
		$(MOC) ../include/parea.h -o moc_parea.cc

moc_qpl.o:	moc_qpl.cc \
		../include/qpl.h

moc_qpl.cc: 	../include/qpl.h
		$(MOC) ../include/qpl.h -o moc_qpl.cc

moc_qpl1.o:     moc_qpl1.cc \
		../include/qpl1.h

moc_qpl1.cc:    ../include/qpl1.h
		$(MOC) ../include/qpl1.h -o moc_qpl1.cc

qm.moc:		qm.cc
		$(MOC) qm.cc -o qm.moc

qm: qm.moc $(QMOBJS) $(QMHEADERS)
	g++ $(LDFLAGS) -o qm $(QMOBJS) $(LOADLIBES)

tmp-install: qpl
	cp qpl $(HOME)/bin

install: qpl
	install -s -m 755 -o 0 -g 0	qpl	/usr/local/bin/

clean:
#	rm -f back.tar.gz
#	tar czf back.tar.gz *
#	if test ! -e ../tmp ; then mkdir ../tmp ; fi
#	if test -d ../tmp ; then mv back.tar.gz ../tmp/ ; fi
	rm -rf *.o moc_* qpl qm qm.moc *~

