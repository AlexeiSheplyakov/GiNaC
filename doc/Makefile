# Generated automatically from Makefile.in by configure.
# This is the prototype Makefile for all of GiNaC's documentation.  It tries to
# be tolerant---if some tools were not found it skips the built of that part
# of documentation and tries to continue.

# Substitution variables from configure skript:
# Here come the usual install directories in GNU-configure fashion:
prefix = /usr/local
docdir = ${prefix}/share/doc/GiNaC
srcdir = .
# Dimitri van Heesch's Doxygen is needed for developer's resource:
DOXYGEN = /usr/local/bin_i386/doxygen
# LaTeX and friends are needed for processing developer's resource for print:
LATEX = /usr/local/bin/latex
MAKEINDEX = /usr/local/bin/makeindex
DVIPS = /usr/local/bin/dvips
FIG2DEV = /usr/bin/X11/fig2dev
# Jade and friends needed for generating the tutorial:
JADE = /usr/bin/jade
JADETEX = /usr/local/bin/jadetex
# Arguments for Jade and friends (does anybody know how to find them out?)
# JADEARGS_TEX = -t tex -o tutorial.tex -d /usr/lib/dsssl/stylesheets/docbook/print/docbook.dsl
JADEARGS_TEX = -t tex -o tutorial.tex -d /usr/lib/sgml/stylesheet/dsssl/docbook/nwalsh/print/docbook.dsl
# JADEARGS_HTML = -t sgml -d /usr/lib/dsssl/stylesheets/docbook/html/docbook.dsl
JADEARGS_HTML = -t sgml -d /usr/lib/sgml/stylesheet/dsssl/docbook/nwalsh/html/docbook.dsl
# Autoconf macro AC_PROC_INSTALL sets these:
INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
# All LaTeX builds will take place in a separate subdirectory:
VPATH = .:./latex:./tutorial

# default target:
all: reference tutorial

# REFERENCE:
# This section produces HTML'ed and TeX'ed developer's reference from the
# sources with a JavaDoc-like tool (in this case doxygen):
reference_html:
	@ if [ ! -d ${srcdir}/reference ]; then mkdir ${srcdir}/reference; fi
	@ if [ "${DOXYGEN}" ]; then \
		echo "Running ${DOXYGEN} ./DoxyfileHTML..."; \
		cd ..; ${DOXYGEN} ./doc/DoxyfileHTML; \
	else \
		echo "warning: target reference_html disabled by configuration"; \
	fi

reference.tex:
	@ if [ ! -d ${srcdir}/latex ]; then mkdir ${srcdir}/latex; fi
	@ if [ "${DOXYGEN}" ]; then \
		echo "Running ${DOXYGEN} ./DoxyfileTEX..."; \
		cd ..; ${DOXYGEN} ./doc/DoxyfileTEX; \
	else \
		echo "warning: target reference.tex disabled by configuration"; \
	fi
	- mv ${srcdir}/latex/refman.tex ${srcdir}/latex/reference.tex 

reference.dvi: reference.tex
	@ if [ "${LATEX}" -a "${MAKEINDEX}" ]; then \
		cd latex; \
		${LATEX} reference.tex && \
		${MAKEINDEX} reference.idx && \
		${LATEX} reference.tex; \
	else \
		echo "warning: target reference.dvi disabled by configuration"; \
	fi

reference.ps: reference.dvi
	@ if [ "${DVIPS}" ]; then \
		echo "Running ${DVIPS} -o reference.ps reference.dvi..."; \
		cd latex; ${DVIPS} -o reference.ps reference.dvi; \
	else \
		echo "warning: target reference.ps disabled by configuration"; \
	fi

reference: reference_html reference.ps

# TUTORIAL:
# This section produces HTML'ed and TeX'ed versions of the tutorial using a
# SGML to TeX converter (in this case jade).  As a dirty hack, we are doing
# some regexpese to tutorial.sgml prior to parsing it, in order to allow for
# different graphics output.  This seems to be an ugly limitation of docbook...

EPS = classhierarchy.eps rep_naive.eps rep_pair.eps
PNG = classhierarchy.png rep_naive.png rep_pair.png

tutorial/index.html: tutorial.sgml.in ${PNG}
	@ if [ ! -d ${srcdir}/latex ]; then mkdir ${srcdir}/latex; fi
	@ if [ "${JADE}" ]; then \
		sed -e 's/graext/png/g' -e 's/GRAEXT/GIF/g' tutorial.sgml.in > tutorial.sgml; \
		echo "Running ${JADE} ${JADEARGS_HTML} tutorial.sgml..."; \
		cd tutorial/; ${JADE} ${JADEARGS_HTML} ../tutorial.sgml; \
		if [ -f book1.html ]; then cp book1.html index.html; fi; \
	else \
		echo "warning: target tutorial_html disabled by configuration"; \
	fi

tutorial.tex: tutorial.sgml.in ${EPS}
	@ if [ ! -d ${srcdir}/latex ]; then mkdir ${srcdir}/latex; fi
	@ if [ "${JADE}" -a "${LATEX}" ]; then \
		sed -e 's/graext/eps/g' -e 's/GRAEXT/EPS/g' tutorial.sgml.in > tutorial.sgml; \
		echo "Running ${JADE} ${JADEARGS_TEX} tutorial.sgml..."; \
		cd latex; ${JADE} ${JADEARGS_TEX} ../tutorial.sgml ; \
	else \
		echo "warning: target tutorial.tex disabled by configuration"; \
	fi

tutorial.dvi: tutorial.tex
	@ if [ "${JADETEX}" ]; then \
		echo "Running ${JADETEX} tutorial.tex..."; \
		cd latex; ${JADETEX} tutorial.tex && ${JADETEX} tutorial.tex && ${JADETEX} tutorial.tex; \
	else \
		echo "warning: target tutorial.dvi disabled by configuration"; \
	fi

tutorial.ps: tutorial.dvi
	@ if [ "${DVIPS}" ]; then \
		echo "Running ${DVIPS} tutorial.dvi -o tutorial.ps..."; \
		cd latex; ${DVIPS} tutorial.dvi -o tutorial.ps; \
	else \
		echo "warning: target tutorial.ps disabled by configuration"; \
	fi

tutorial:	tutorial/index.html tutorial.ps

# Target for installing all generated documentation files on the system.
# (Foolproof: installs what we got, even if one or more targets failed.)
install:
	- ${INSTALL} -d ${docdir}/reference/
	- ${INSTALL_DATA} ${srcdir}/reference/* ${docdir}/reference/
	- ${INSTALL_DATA} ${srcdir}/latex/reference.ps ${docdir}/
	- ${INSTALL} -d ${docdir}/tutorial/
	- ${INSTALL_DATA} ${srcdir}/tutorial/* ${docdir}/tutorial/
	- ${INSTALL_DATA} ${srcdir}/latex/tutorial.ps ${docdir}/

# Removes all installed documentation files from the system:
uninstall:
	rm -rf ${docdir}

# The next targets should only be called in case of emergency by developers, 
# since the complete documentation is not normally rebuilt.  In any case, they
# should only be called from people who know what they are doing and never 
# from top-level Makefile's targets clean and distclean.
clean:
	rm -rf ${srcdir}/tutorial ${srcdir}/reference ${srcdir}/latex
	rm -f tutorial.sgml

distclean: clean
	rm -f Makefile

# Special dummy targets:
.SUFFIXES: .fig .eps .png
.PHONY: all clean distclean reference tutorial
.SILENT: all reference tutorial_html tutorial_tex

# Suffix rules:
.fig.eps :
	@ if [ ! -d ${srcdir}/latex ]; then mkdir ${srcdir}/latex; fi
	if [ "${FIG2DEV}" ]; then \
		${FIG2DEV} -L ps -m 0.8 $< ${srcdir}/latex/$@; \
	else \
		echo "warning: fig2dev was not found by configure"; \
	fi
.fig.png :
	@ if [ ! -d ${srcdir}/tutorial ]; then mkdir ${srcdir}/tutorial; fi
	if [ "${FIG2DEV}" ]; then \
		${FIG2DEV} -L png $< ${srcdir}/tutorial/$@; \
	else \
		echo "warning: fig2dev was not found by configure"; \
	fi
