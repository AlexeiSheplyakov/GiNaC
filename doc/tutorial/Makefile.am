## Process this file with automake to produce Makefile.in

# Graphics file conversion
FIG = classhierarchy.fig repnaive.fig reppair.fig repreal.fig
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

info_TEXINFOS = ginac.texi

ginac.dvi: $(EPS)

ginac.html: ginac.texi version.texi $(PNG)
	cd $(srcdir) \
	  && $(MAKEINFO) --html `echo $< | sed 's,.*/,,'`

ginacdocdir = $(datadir)/doc/GiNaC
ginactutorialdir = $(ginacdocdir)/tutorial
ginactutorial_DATA = @TUTORIAL_TARGETS@

CLEANFILES = $(EPS) $(PNG) *.html
EXTRA_DIST = $(FIG)

#CLEANFILES = *.html *.png *.ps *.tex *.log *.aux *.dvi *.eps tutorial.sgml HTML.manifest

#install-data-local:
#	@for p in *.html; do \
#	  echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(ginactutorialdir)/$$p"; \
#	  $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(ginactutorialdir)/$$p; \
#	done
#	@for p in *.png; do \
#	  echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(ginactutorialdir)/$$p"; \
#	  $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(ginactutorialdir)/$$p; \
#	done

#uninstall-local:
#	@for p in *.html; do \
#	  echo " rm -f $(DESTDIR)$(ginactutorialdir)/$$p"; \
#	  rm -f $(DESTDIR)$(ginactutorialdir)/$$p; \
#	done
#	@for p in *.png; do \
#	  echo " rm -f $(DESTDIR)$(ginactutorialdir)/$$p"; \
#	  rm -f $(DESTDIR)$(ginactutorialdir)/$$p; \
#	done
