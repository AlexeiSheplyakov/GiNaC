## Process this file with automake to produce Makefile.in
bin_PROGRAMS = ginsh
ginsh_SOURCES = ginsh_parser.yy ginsh_lexer.ll ginsh.h
ginsh_LDADD = ../ginac/libginac.la $(GINSH_LIBS)
man_MANS = ginsh.1
YFLAGS = -d
EXTRA_DIST = ginsh_parser.h
