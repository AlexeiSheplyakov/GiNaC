## Process this file with automake to produce Makefile.in

# Targets
html: html_files/index.html
dvi: reference.dvi
ps: reference.ps
pdf: reference.pdf

# This section produces HTML'ed and TeX'ed developer's reference from the
# sources with a JavaDoc-like tool (in this case doxygen):
html_files::
	@[ -d html_files ] || mkdir html_files >/dev/null 2>&1

html_files/index.html:
	@if [ "${DOXYGEN}" ]; then \
	  echo "Running ${DOXYGEN} DoxyfileHTML..."; \
	  ${DOXYGEN} DoxyfileHTML; \
	else \
	  echo "warning: target reference_html disabled by configuration"; \
	fi

latex::
	@[ -d latex ] || mkdir latex >/dev/null 2>&1

latex/reference.tex:
	@if [ "${DOXYGEN}" ]; then \
	  echo "Running ${DOXYGEN} DoxyfileTEX..."; \
	  ${DOXYGEN} DoxyfileTEX; \
	  mv latex/refman.tex latex/reference.tex; \
	else \
	  echo "warning: target reference.tex disabled by configuration"; \
	fi

latex/reference.dvi: latex/reference.tex
	@if [ "${LATEX}" -a "${MAKEINDEX}" ]; then \
	  (cd latex; \
	  ${LATEX} reference.tex && \
	  ${MAKEINDEX} reference.idx && \
	  ${LATEX} reference.tex); \
	else \
	  echo "warning: target reference.dvi disabled by configuration"; \
	fi

latex/reference.ps: latex/reference.dvi
	@if [ "${DVIPS}" ]; then \
	  echo "Running ${DVIPS} -o reference.ps reference.dvi..."; \
	  (cd latex; ${DVIPS} -o reference.ps reference.dvi); \
	else \
	  echo "warning: target reference.ps disabled by configuration"; \
	fi

pdflatex::
	@[ -d pdflatex ] || mkdir pdflatex >/dev/null 2>&1

pdflatex/reference.tex:
	@if [ "${DOXYGEN}" ]; then \
	  echo "Running ${DOXYGEN} DoxyfilePDF..."; \
	  ${DOXYGEN} DoxyfilePDF; \
	  mv pdflatex/refman.tex pdflatex/reference.tex; \
	else \
	  echo "warning: target reference.pdf disabled by configuration"; \
	fi

pdflatex/reference.pdf: pdflatex/reference.tex
	@if [ "${PDFLATEX}" -a "${MAKEINDEX}" ]; then \
	  (cd pdflatex; \
	  ${PDFLATEX} reference.tex && \
	  ${MAKEINDEX} reference.idx && \
	  ${PDFLATEX} reference.tex); \
	else \
	  echo "warning: target reference.pdf disabled by configuration"; \
	fi

reference.dvi: latex latex/reference.dvi
	cp latex/reference.dvi reference.dvi

reference.ps: latex latex/reference.ps
	cp latex/reference.ps reference.ps

reference.pdf: pdflatex pdflatex/reference.pdf
	cp pdflatex/reference.pdf reference.pdf

CLEANFILES = reference.dvi reference.ps reference.pdf html_files/* latex/* pdflatex/*
