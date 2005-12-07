## Process this file with automake to produce Makefile.in

bin_PROGRAMS = viewgar
viewgar_SOURCES = viewgar.cpp
viewgar_LDADD = ../ginac/libginac.la

bin_SCRIPTS = ginac-excompiler

INCLUDES = -I$(srcdir)/../ginac -I../ginac

man_MANS = viewgar.1

EXTRA_DIST = ginac-excompiler.in
