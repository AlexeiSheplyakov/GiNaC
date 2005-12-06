## Process this file with automake to produce Makefile.in

EXFILES = archive1.cpp

TEXINFO_TEX = ../tutorial/texinfo.tex

info_TEXINFOS = ginac-examples.texi

all: $(EXFILES)
	${MAKEINFO} --no-split --no-headers $(srcdir)/ginac-examples.texi > ginac-examples.txt
	${MAKEINFO} --no-split --html $(srcdir)/ginac-examples.texi

EXTRA_DIST = $(EXFILES)
