## Process this file with automake to produce Makefile.in
TESTS = run_exams run_checks run_times
check_PROGRAMS = exams checks times
exams_SOURCES = exam_paranoia.cpp exam_numeric.cpp exam_powerlaws.cpp \
  exam_differentiation.cpp exam_polygcd.cpp exam_normalization.cpp \
  exam_pseries.cpp exam_matrices.cpp exam_lsolve.cpp exam_noncommut.cpp \
  exam_misc.cpp exams.cpp exams.h
exams_LDADD = ../ginac/libginac.la
checks_SOURCES =  check_numeric.cpp check_inifcns.cpp check_matrices.cpp \
  check_lsolve.cpp genex.cpp checks.cpp checks.h
checks_LDADD = ../ginac/libginac.la
times_SOURCES = time_dennyfliegner.cpp time_gammaseries.cpp timer.cpp \
  times.cpp times.h
times_LDADD = ../ginac/libginac.la
INCLUDES = -I$(srcdir)/../ginac
CLEANFILES = exams.out checks.out times.out
EXTRA_DIST = exams.ref checks.ref times.ref run_exams run_checks run_times
