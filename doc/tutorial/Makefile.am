## Process this file with automake to produce Makefile.in

# Graphics file conversion
FIG = classhierarchy.fig repnaive.fig reppair.fig repreal.fig
PNG = $(addsuffix .png, $(basename $(FIG)))
EPS = $(addsuffix .eps, $(basename $(FIG)))
TXT = $(addsuffix .txt, $(basename $(FIG)))

SUFFIXES = .fig .png .eps .txt

%.eps: %.fig
	@if [ "${FIG2DEV}" ]; then \
	  echo "Running ${FIG2DEV} -L eps -m 0.9 $< $@..."; \
	  ${FIG2DEV} -L eps -m 0.9 $< $@; \
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

CLEANFILES = $(EPS) $(PNG) ginac.html
EXTRA_DIST = $(FIG) $(TXT)

install-data-local:
	@for p in $(PNG); do \
          if [ -f $(srcdir)/$$p ]; then \
	    echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(ginactutorialdir)/$$p"; \
	    $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(ginactutorialdir)/$$p; \
	  fi; \
	done

uninstall-local:
	@for p in $(PNG); do \
	  echo " rm -f $(DESTDIR)$(ginactutorialdir)/$$p"; \
	  rm -f $(DESTDIR)$(ginactutorialdir)/$$p; \
	done
