## Process this file with automake to produce Makefile.in

bin_PROGRAMS = ginaccint.bin ginaccint
check_PROGRAMS = run_exams
run_exams_SOURCES = run_exams.cpp
ginaccint_SOURCES = ginaccint.cpp
ginaccint_bin_SOURCES = ginaccint.bin.cpp
man_MANS = ginaccint.1

ginaccint.bin: ginaccint.bin.cpp
	(export CINTSYSDIR=$(CINTSYSDIR); $(MAKE) -f Makefile.makecint PATH=$(PATH):$(CINTSYSDIR))

TESTS = exam_paranoia.cpp exam_mansamples.cpp exam_misc.cpp

EXTRA_DIST = $(man_MANS) $(TESTS)

# Force targets clean and distclean to call Makecint's own Makefile
makecint_clean:
	(if [ -f Makefile.makecint ]; then $(MAKE) -f Makefile.makecint clean; fi)

clean: makecint_clean

distclean: makecint_clean
	rm -f Makefile.makecint
