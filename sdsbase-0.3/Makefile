VERSION=0.3
RELEASE=10
RSDSDIR=sdsbase-$(VERSION)

all:
	$(MAKE) -C lib.src tmp-install
	$(MAKE) -C src
	$(MAKE) -C tool.src all
	$(MAKE) -C dssc
	$(MAKE) -C manip
	$(MAKE) -C dsdel
	$(MAKE) -C dsn
	$(MAKE) -C dsreg
	$(MAKE) -C dsl
	$(MAKE) -C dslold
	$(MAKE) -C inform

tar: 
	rm -rf $(RSDSDIR).tar.gz
	(cd .. ; tar czf $(RSDSDIR)/$(RSDSDIR).tar.gz\
				 $(RSDSDIR)/README.qt2\
				 $(RSDSDIR)/Makefile $(RSDSDIR)/sdsbase.spec\
				 $(RSDSDIR)/include $(RSDSDIR)/lib.src\
				 $(RSDSDIR)/src $(RSDSDIR)/tool.src\
				 $(RSDSDIR)/dssc $(RSDSDIR)/dsdel\
				 $(RSDSDIR)/dsreg $(RSDSDIR)/dsl\
				 $(RSDSDIR)/dslold $(RSDSDIR)/manip\
				 $(RSDSDIR)/inform $(RSDSDIR)/lib\
				 $(RSDSDIR)/dsn)

clean:
	find . -name "*.d" -exec rm -f \{\} \;
	$(MAKE) -C lib.src clean
	$(MAKE) -C lib clean
	$(MAKE) -C src clean
	$(MAKE) -C tool.src clean
	$(MAKE) -C include clean
	$(MAKE) -C dssc clean
	$(MAKE) -C dsdel clean
	$(MAKE) -C dsn clean
	$(MAKE) -C dsreg clean
	$(MAKE) -C dsl clean
	$(MAKE) -C dslold clean
	$(MAKE) -C manip clean
	$(MAKE) -C inform clean
	rm -f *~

almostclean:
	$(MAKE) -C src clean
	$(MAKE) -C tool.src clean
	$(MAKE) -C include clean
	$(MAKE) -C dssc clean
	$(MAKE) -C dsdel clean
	$(MAKE) -C dsn clean
	$(MAKE) -C dsreg clean
	$(MAKE) -C dsl clean
	$(MAKE) -C dslold clean
	$(MAKE) -C manip clean
	$(MAKE) -C inform clean

progs:
	$(MAKE) -C src
	$(MAKE) -C tool.src all
	$(MAKE) -C dssc
	$(MAKE) -C dsdel
	$(MAKE) -C dsn
	$(MAKE) -C dsreg
	$(MAKE) -C dsl
	$(MAKE) -C dslold
	$(MAKE) -C manip
	$(MAKE) -C inform

#redo: almostclean progs
redo: clean tmp-install

tmp-install:
	$(MAKE) -C lib.src tmp-install
	$(MAKE) -C src tmp-install
	$(MAKE) -C tool.src tmp-install
	$(MAKE) -C dssc tmp-install
	$(MAKE) -C dsdel tmp-install
	$(MAKE) -C dsn tmp-install
	$(MAKE) -C dsreg tmp-install
	$(MAKE) -C dsl tmp-install
	$(MAKE) -C dslold tmp-install
	$(MAKE) -C manip tmp-install
	$(MAKE) -C inform tmp-install

install:
	$(MAKE) -C lib.src install
	$(MAKE) -C include install
	$(MAKE) -C src install
	$(MAKE) -C tool.src install
	$(MAKE) -C dssc install
	$(MAKE) -C dsdel install
	$(MAKE) -C dsn install
	$(MAKE) -C dsreg install
	$(MAKE) -C dsl install
	$(MAKE) -C dslold install
	$(MAKE) -C manip install
	$(MAKE) -C inform install

copy: tar
	cksum $(RSDSDIR).tar.gz >cksum
	mcopy -o $(RSDSDIR).tar.gz cksum a:tmp 
	mcopy -o $(RSDSDIR).tar.gz cksum a: 
	rm cksum

dist: tar
	cp $(RSDSDIR).tar.gz /usr/src/packages/SOURCES
	(cd /usr/src/packages/SOURCES; rpm -ta $(RSDSDIR).tar.gz)
	cp /usr/src/packages/SRPMS/$(RSDSDIR)-$(RELEASE).src.rpm .
	cp /usr/src/packages/RPMS/i386/$(RSDSDIR)-$(RELEASE).i386.rpm .
