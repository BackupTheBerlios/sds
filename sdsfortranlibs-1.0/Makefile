VERSION=1.0
RELEASE=8
SDSFORTRANLIBS=sdsfortranlibs-$(VERSION)

all:
	$(MAKE) -C spline tmp-install
	$(MAKE) -C approx tmp-install
	$(MAKE) -C fn tmp-install

tar: 
	rm -rf $(SDSFORTRANLIBS).tar.gz
	(cd .. ; tar czf $(SDSFORTRANLIBS)/$(SDSFORTRANLIBS).tar.gz\
				 $(SDSFORTRANLIBS)/Makefile\
				 $(SDSFORTRANLIBS)/sdsfortranlibs.spec\
				 $(SDSFORTRANLIBS)/include\
				 $(SDSFORTRANLIBS)/lib\
				 $(SDSFORTRANLIBS)/spline\
				 $(SDSFORTRANLIBS)/approx\
				 $(SDSFORTRANLIBS)/fn)


clean:
	$(MAKE) -C include clean
	$(MAKE) -C fn clean
	$(MAKE) -C approx clean
	$(MAKE) -C spline clean
	$(MAKE) -C lib clean
	rm -f *~

install:
	$(MAKE) -C spline install
	$(MAKE) -C approx install
	$(MAKE) -C fn install

copy: tar
	cksum $(SDSFORTRANLIBS).tar.gz >cksum
	mcopy -o $(SDSFORTRANLIBS).tar.gz cksum a:tmp 
	mcopy -o $(SDSFORTRANLIBS).tar.gz cksum a: 
	rm cksum

dist: tar
	cp $(SDSFORTRANLIBS).tar.gz /usr/src/packages/SOURCES
	(cd /usr/src/packages/SOURCES; rpm -ta $(SDSFORTRANLIBS).tar.gz)
	cp /usr/src/packages/SRPMS/$(SDSFORTRANLIBS)-$(RELEASE).src.rpm .
	cp /usr/src/packages/RPMS/i386/$(SDSFORTRANLIBS)-$(RELEASE).i386.rpm .
