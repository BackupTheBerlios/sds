VERSION=0.4
RELEASE=2
SDSPROG=sdsprog1-$(VERSION)

all:
	$(MAKE) -C dsspline
	$(MAKE) -C dsfit
	$(MAKE) -C dsadd
	$(MAKE) -C dsmul
	$(MAKE) -C dsft
	$(MAKE) -C dswl
	$(MAKE) -C dspeak
	$(MAKE) -C dscf
	$(MAKE) -C dsxfit
	$(MAKE) -C dsdydx
	$(MAKE) -C dsxy

tar: 
	rm -rf $(SDSPROG).tar.gz
	(cd .. ; tar czf $(SDSPROG)/$(SDSPROG).tar.gz\
				 $(SDSPROG)/Makefile $(SDSPROG)/sdsprog1.spec\
				 $(SDSPROG)/dsspline $(SDSPROG)/dsfit\
				 $(SDSPROG)/dswl $(SDSPROG)/dsmul\
				 $(SDSPROG)/dspeak\
				 $(SDSPROG)/dsadd $(SDSPROG)/dsft\
				 $(SDSPROG)/dscf $(SDSPROG)/dsxy\
				 $(SDSPROG)/dsxfit $(SDSPROG)/dsdydx)


clean:
	$(MAKE) -C dsspline clean
	$(MAKE) -C dsfit clean
	$(MAKE) -C dsadd clean
	$(MAKE) -C dsmul clean
	$(MAKE) -C dsft clean
	$(MAKE) -C dswl clean
	$(MAKE) -C dspeak clean
	$(MAKE) -C dscf clean
	$(MAKE) -C dsxfit clean
	$(MAKE) -C dsdydx clean
	$(MAKE) -C dsxy clean
	rm -f *~

progs:
	$(MAKE) -C dsspline
	$(MAKE) -C dsfit
	$(MAKE) -C dsadd
	$(MAKE) -C dsmul
	$(MAKE) -C dsft
	$(MAKE) -C dswl
	$(MAKE) -C dspeak
	$(MAKE) -C dscf
	$(MAKE) -C dsxfit
	$(MAKE) -C dsdydx
	$(MAKE) -C dsxy


redo: clean progs

install:
	$(MAKE) -C dsspline install
	$(MAKE) -C dsfit install
	$(MAKE) -C dsadd install
	$(MAKE) -C dsmul install
	$(MAKE) -C dsft install
	$(MAKE) -C dswl install
	$(MAKE) -C dspeak install
	$(MAKE) -C dscf install
	$(MAKE) -C dsxfit install
	$(MAKE) -C dsdydx install
	$(MAKE) -C dsxy install

tmp-install:
	$(MAKE) -C dsspline tmp-install
	$(MAKE) -C dsfit tmp-install
	$(MAKE) -C dsadd tmp-install
	$(MAKE) -C dsmul tmp-install
	$(MAKE) -C dsft tmp-install
	$(MAKE) -C dswl tmp-install
	$(MAKE) -C dspeak tmp-install
	$(MAKE) -C dscf tmp-install
	$(MAKE) -C dsxfit tmp-install
	$(MAKE) -C dsdydx tmp-install
	$(MAKE) -C dsxy tmp-install


copy: tar
	cksum $(SDSPROG).tar.gz >cksum
	mcopy -o $(SDSPROG).tar.gz cksum a:tmp 
	mcopy -o $(SDSPROG).tar.gz cksum a: 
	rm cksum

dist: tar
	cp $(SDSPROG).tar.gz /usr/src/packages/SOURCES
	(cd /usr/src/packages/SOURCES; rpm -ta $(SDSPROG).tar.gz)
	cp /usr/src/packages/SRPMS/$(SDSPROG)-$(RELEASE).src.rpm .
	cp /usr/src/packages/RPMS/i386/$(SDSPROG)-$(RELEASE).i386.rpm .
