## Process this file with automake to produce Makefile.in

# This section produces HTML'ed and TeX'ed versions of the tutorial using a
# SGML to TeX converter (in this case jade).  As a dirty hack, we are doing
# some regexpese to tutorial.sgml prior to parsing it, in order to allow for
# different graphics output.  This seems to be an ugly limitation of docbook...

FIG = classhierarchy.fig rep_naive.fig rep_pair.fig rep_real.fig
PNG = $(addsuffix .png, $(basename $(FIG)))
EPS = $(addsuffix .eps, $(basename $(FIG)))

SUFFIXES = .fig .png .eps

%.eps: %.fig
	@if [ "${FIG2DEV}" ]; then \
	  echo "Running ${FIG2DEV} -L ps -m 0.8 $< $@..."; \
	  ${FIG2DEV} -L ps -m 0.8 $< $@; \
	else \
	  echo "warning: fig2dev was not found by configure"; \
	fi

%.png: %.fig
	@if [ "${FIG2DEV}" ]; then \
	  echo "Running ${FIG2DEV} -L png $< $@..."; \
	  ${FIG2DEV} -L png $< $@; \
	else \
	  echo "warning: fig2dev was not found by configure"; \
	fi

JADEARGS_TEX = -t tex -o tutorial.tex -d /usr/lib/sgml/stylesheet/dsssl/docbook/nwalsh/print/docbook.dsl
JADEARGS_HTML = -t sgml -d /usr/lib/sgml/stylesheet/dsssl/docbook/nwalsh/html/docbook.dsl

index.html: tutorial.sgml.in $(PNG)
	@if [ "${JADE}" ]; then \
	  sed -e "s/MAJOR_VERSION/${GINACLIB_MAJOR_VERSION}/" \
	      -e "s/MINOR_VERSION/${GINACLIB_MINOR_VERSION}/" \
	      -e 's/graext/png/g' \
	      -e 's/GRAEXT/GIF/g' tutorial.sgml.in > tutorial.sgml; \
	  echo "Running ${JADE} ${JADEARGS_HTML} tutorial.sgml..."; \
	  ${JADE} ${JADEARGS_HTML} tutorial.sgml; \
	  if [ -f book1.html ]; then mv book1.html index.html; fi; \
	else \
	  echo "warning: target tutorial_html disabled by configuration"; \
	fi

tutorial.tex: tutorial.sgml.in $(EPS)
	@if [ "${JADE}" -a "${LATEX}" ]; then \
	  sed -e "s/MAJOR_VERSION/${GINACLIB_MAJOR_VERSION}/" \
	      -e "s/MINOR_VERSION/${GINACLIB_MINOR_VERSION}/" \
	      -e 's/graext/eps/g' \
	      -e 's/GRAEXT/EPS/g' tutorial.sgml.in > tutorial.sgml; \
	  echo "Running ${JADE} ${JADEARGS_TEX} tutorial.sgml..."; \
	  ${JADE} ${JADEARGS_TEX} tutorial.sgml ; \
	else \
	  echo "warning: target tutorial.tex disabled by configuration"; \
	fi

tutorial.dvi: tutorial.tex
	@if [ "${JADETEX}" ]; then \
	  echo "Running ${JADETEX} tutorial.tex..."; \
	  ${JADETEX} tutorial.tex && ${JADETEX} tutorial.tex && ${JADETEX} tutorial.tex; \
	else \
	  echo "warning: target tutorial.dvi disabled by configuration"; \
	fi

tutorial.ps: tutorial.dvi
	@if [ "${DVIPS}" ]; then \
	  echo "Running ${DVIPS} tutorial.dvi -o tutorial.ps..."; \
	  ${DVIPS} tutorial.dvi -o tutorial.ps; \
	else \
	  echo "warning: target tutorial.ps disabled by configuration"; \
	fi

install-data-local:
	@for p in *.html; do \
	  echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(ginactutorialdir)/$$p"; \
	  $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(ginactutorialdir)/$$p; \
	done
	@for p in *.png; do \
	  echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(ginactutorialdir)/$$p"; \
	  $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(ginactutorialdir)/$$p; \
	done

uninstall-local:
	@for p in *.html; do \
	  echo " rm -f $(DESTDIR)$(ginactutorialdir)/$$p"; \
	  rm -f $(DESTDIR)$(ginactutorialdir)/$$p; \
	done
	@for p in *.png; do \
	  echo " rm -f $(DESTDIR)$(ginactutorialdir)/$$p"; \
	  rm -f $(DESTDIR)$(ginactutorialdir)/$$p; \
	done

ginacdocdir = $(datadir)/doc/GiNaC
ginactutorialdir = $(ginacdocdir)/tutorial
ginactutorial_DATA = $(TUTORIAL_TARGETS)
EXTRA_DATA = index.html tutorial.ps

CLEANFILES = *.html *.png *.ps *.tex *.log *.aux *.dvi *.eps tutorial.sgml HTML.manifest
EXTRA_DIST = tutorial.sgml.in $(FIG)
