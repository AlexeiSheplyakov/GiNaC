## Process this file with automake to produce Makefile.in

# This section produces HTML'ed and TeX'ed developer's reference from the
# sources with a JavaDoc-like tool (in this case doxygen):
index.html:
	@if [ "${DOXYGEN}" ]; then \
	  echo "Running ${DOXYGEN} DoxyfileHTML..."; \
	  (cd ${srcdir}; \
	  ${DOXYGEN} DoxyfileHTML); \
	else \
	  echo "warning: target reference_html disabled by configuration"; \
	fi

latex::
	@[ -d ${srcdir}/latex ] || mkdir ${srcdir}/latex >/dev/null 2>&1

latex/reference.tex:
	@if [ "${DOXYGEN}" ]; then \
	  echo "Running ${DOXYGEN} DoxyfileTEX..."; \
	  (cd ${srcdir}; \
	  ${DOXYGEN} DoxyfileTEX); \
	  mv ${srcdir}/latex/refman.tex ${srcdir}/latex/reference.tex; \
	else \
	  echo "warning: target reference.tex disabled by configuration"; \
	fi

latex/reference.dvi: latex/reference.tex
	@if [ "${LATEX}" -a "${MAKEINDEX}" ]; then \
	  (cd ${srcdir}/latex; \
	  ${LATEX} reference.tex && \
	  ${MAKEINDEX} reference.idx && \
	  ${LATEX} reference.tex); \
	else \
	  echo "warning: target reference.dvi disabled by configuration"; \
	fi

latex/reference.ps: latex/reference.dvi
	@if [ "${DVIPS}" ]; then \
	  echo "Running ${DVIPS} -o reference.ps reference.dvi..."; \
	  (cd ${srcdir}/latex; ${DVIPS} -o reference.ps reference.dvi); \
	else \
	  echo "warning: target reference.ps disabled by configuration"; \
	fi

reference.ps: latex latex/reference.ps
	cp ${srcdir}/latex/reference.ps reference.ps

ginacreferencedir = `test -z "@REFERENCE_TARGETS@" || echo @datadir@/doc/GiNaC/reference`
ginacreference_DATA = @REFERENCE_TARGETS@

CLEANFILES = *.html *.png doxygen.css *.ps latex/*
EXTRA_DIST = DoxyfileHTML DoxyfileTEX Doxyfooter

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(ginacreferencedir)
	@if [ -f $(srcdir)/doxygen.css ]; then \
	  echo " $(INSTALL_DATA) $(srcdir)/doxygen.css $(DESTDIR)$(ginacreferencedir)/doxygen.css"; \
	  $(INSTALL_DATA) $(srcdir)/doxygen.css $(DESTDIR)$(ginacreferencedir)/doxygen.css; \
	fi
	@for p in `echo $(srcdir)/*.html | sed -e 's,[^ ]*/,,g' -e 's,\*\.html,,'`; do \
	  echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(ginacreferencedir)/$$p"; \
	  $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(ginacreferencedir)/$$p; \
	done
	@for p in `echo $(srcdir)/*.png | sed -e 's,[^ ]*/,,g' -e 's,\*\.png,,'`; do \
	  echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(ginacreferencedir)/$$p"; \
	  $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(ginacreferencedir)/$$p; \
	done

uninstall-local:
	rm -f $(DESTDIR)$(ginacreferencedir)/doxygen.css
	rm -f $(DESTDIR)$(ginacreferencedir)/*.html
	rm -f $(DESTDIR)$(ginacreferencedir)/*.png
