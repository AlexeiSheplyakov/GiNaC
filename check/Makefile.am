## Process this file with automake to produce Makefile.in

TESTS = run_exams run_checks run_times
check_PROGRAMS = exams checks times
TESTS_ENVIRONMENT = EXEEXT=$(EXEEXT) srcdir=$(srcdir)

checks_SOURCES =  check_numeric.cpp check_inifcns.cpp check_matrices.cpp \
  check_lsolve.cpp genex.cpp checks.cpp checks.h
checks_LDADD = ../ginac/libginac.la

exams_SOURCES = exam_paranoia.cpp exam_numeric.cpp exam_powerlaws.cpp \
  exam_inifcns.cpp exam_inifcns_nstdsums.cpp exam_inifcns_nstdsums.h \
  exam_differentiation.cpp exam_polygcd.cpp exam_normalization.cpp \
  exam_pseries.cpp exam_matrices.cpp exam_lsolve.cpp exam_indexed.cpp \
  exam_color.cpp exam_clifford.cpp exam_archive.cpp exam_structure.cpp \
  exam_hashmap.cpp exam_misc.cpp exams.cpp exams.h
exams_LDADD = ../ginac/libginac.la

times_SOURCES = time_dennyfliegner.cpp time_gammaseries.cpp \
  time_vandermonde.cpp time_toeplitz.cpp time_hashmap.cpp time_lw_A.cpp \
  time_lw_B.cpp time_lw_C.cpp time_lw_D.cpp time_lw_E.cpp time_lw_F.cpp \
  time_lw_G.cpp time_lw_H.cpp time_lw_IJKL.cpp time_lw_M1.cpp time_lw_M2.cpp \
  time_lw_N.cpp time_lw_O.cpp time_lw_P.cpp time_lw_Pprime.cpp time_lw_Q.cpp \
  time_lw_Qprime.cpp time_antipode.cpp time_fateman_expand.cpp timer.cpp \
  timer.h times.cpp times.h
times_LDADD = ../ginac/libginac.la

INCLUDES = -I$(srcdir)/../ginac -I../ginac

CLEANFILES = exams.out checks.out times.out exam.gar
EXTRA_DIST = exams.ref checks.ref times.ref run_exams run_checks run_times
