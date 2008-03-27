## Process this file with automake to produce Makefile.in

FIG = classhierarchy.fig repnaive.fig reppair.fig repreal.fig
info_TEXINFOS =
AM_MAKEINFOHTMLFLAGS = --no-split

if CONFIG_TEX
if CONFIG_FIG2DEV

# Graphics file conversion
PNG = $(addsuffix .png, $(basename $(FIG)))
EPS = $(addsuffix .eps, $(basename $(FIG)))
PDF = $(addsuffix .pdf, $(basename $(FIG)))

SUFFIXES = .fig .png .eps

.fig.eps:
	  echo "Running ${FIG2DEV} -L eps -m 0.9 $< $@..."; \
	  ${FIG2DEV} -L eps -m 0.9 $< $@;

.fig.pdf:
	  echo "Running ${FIG2DEV} -L pdf -m 0.9 $< $@..."; \
	  ${FIG2DEV} -L pdf -m 0.9 $< $@

.fig.png:
	@echo "Running ${FIG2DEV} -L png $< $@..."; \
	  ${FIG2DEV} -L png $< $@


info_TEXINFOS += ginac.texi

ginac.pdf: $(PDF)
ginac.dvi: $(EPS)
ginac.html: $(PNG)
CLEANFILES = $(EPS) $(PDF) $(PNG)
## CONFIG_FIG2DEV
endif
## CONFIG_TEX
endif

EXTRA_DIST = $(FIG)
