TEXI_SRC=sds.texi
TEXI_INCL=intro.texi concepts.texi programs.texi packages.texi \
	  fit.texi

PROGS=dsl dsd_s dsreg dsrm dsg colxy dslog qpl dssp dsf dsn dsstat \
	dsminmax dsmatch dsls dssc dsdel dscat dssort dsfunc dseinige \
	dsaddmul dsint dsfilter dshist dspad dsdeglitch \
        dsfit dspeak dsxfit dsspline dsdydx dsft dsaco \
        dswl dscf

TEXI_PRG=$(PROGS:%=programs/%.texi)

TEXI=$(TEXI_SRC) $(TEXI_INCL) $(TEXI_PRG)

all:  sds.info sds.html sds.dvi sds.pdf

sds.info: $(TEXI)

sds.html: $(TEXI)
	makeinfo --html $<

sds.dvi: $(TEXI)
	texi2dvi -t @afourlatex $<

sds.pdf: $(TEXI)
	texi2dvi -t @afourlatex -p $<

install: sds.info
	install-info $< dir

tmp-clean:
	rm -f *.{aux,cp,fn,ky,log,pg,toc,tp,vr,cps,pgs,vrs} *~ \
	      $(TEXI_INCL:%=%~)

clean: tmp-clean
	rm -f sds.{info*,html,dvi,pdf}

uninstall:
	rm -f dir

