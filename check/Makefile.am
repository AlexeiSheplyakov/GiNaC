## Process this file with automake to produce Makefile.in
noinst_PROGRAMS = check_ginac
check_ginac_SOURCES = paranoia_check.cpp numeric_output.cpp numeric_consist.cpp \
  powerlaws.cpp expand_subs.cpp inifcns_consist.cpp differentiation.cpp \
  poly_gcd.cpp normalization.cpp lsolve_onedim.cpp matrix_checks.cpp \
  series_expansion.cpp fcntimer.cpp main.cpp
check_ginac_LDADD = ../ginac/libginac.la
TESTS = run_checks
