## Process this file with automake to produce Makefile.in

AM_MAKEINFOHTMLFLAGS = --no-split

# Graphics file conversion
FIG = classhierarchy.fig repnaive.fig reppair.fig repreal.fig
PNG = $(addsuffix .png, $(basename $(FIG)))
EPS = $(addsuffix .eps, $(basename $(FIG)))
PDF = $(addsuffix .pdf, $(basename $(FIG)))
TXT = $(addsuffix .txt, $(basename $(FIG)))

SUFFIXES = .fig .png .eps .txt

.fig.eps:
	@if [ "${FIG2DEV}" ]; then \
	  echo "Running ${FIG2DEV} -L eps -m 0.9 $< $@..."; \
	  ${FIG2DEV} -L eps -m 0.9 $< $@; \
	else \
	  echo "warning: fig2dev was not found by configure"; \
	fi

.fig.pdf:
	@if [ "${FIG2DEV}" ]; then \
	  echo "Running ${FIG2DEV} -L pdf -m 0.9 $< $@..."; \
	  ${FIG2DEV} -L pdf -m 0.9 $< $@; \
	else \
	  echo "warning: fig2dev was not found by configure"; \
	fi

.fig.png:
	@if [ "${FIG2DEV}" ]; then \
	  echo "Running ${FIG2DEV} -L png $< $@..."; \
	  ${FIG2DEV} -L png $< $@; \
	else \
	  echo "warning: fig2dev was not found by configure"; \
	fi

info_TEXINFOS = ginac.texi

ginac.pdf: $(PDF)
ginac.dvi: $(EPS)
ginac.html: $(PNG)

CLEANFILES = $(EPS) $(PDF) $(PNG)
EXTRA_DIST = $(FIG) $(TXT)
