## Process this file with automake to produce Makefile.in

bin_PROGRAMS = ginaccint.bin
bin_SCRIPTS = ginaccint
ginaccint_bin_SOURCES = ginaccint.bin.cpp
man_MANS = ginaccint.1

ginaccint.bin: ginaccint.bin.cpp
	(export CINTSYSDIR=$(CINTSYSDIR); $(MAKE) -f Makefile.makecint PATH=$(PATH):$(CINTSYSDIR))

EXTRA_DIST = $(man_MANS)

# Force targets clean and distclean to call Makecint's own Makefile
makecint_clean:
	(if [ -f Makefile.makecint ]; then $(MAKE) -f Makefile.makecint clean; fi)

clean: makecint_clean

distclean: makecint_clean
	rm -f Makefile.makecint
