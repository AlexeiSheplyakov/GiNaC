## Process this file with automake to produce Makefile.in
TESTS = run_checks
check_PROGRAMS = check_ginac
check_ginac_SOURCES = paranoia_check.cpp numeric_output.cpp \
  numeric_consist.cpp powerlaws.cpp expand_subs.cpp inifcns_consist.cpp \
  differentiation.cpp poly_gcd.cpp normalization.cpp lsolve_onedim.cpp \
  matrix_checks.cpp series_expansion.cpp lortensor_check.cpp \
  fcntimer.cpp main.cpp check.h
check_ginac_LDADD = ../ginac/libginac.la
CLEANFILES = result.out
EXTRA_DIST = result.ref run_checks
