## Process this file with automake to produce Makefile.in
bin_PROGRAMS = ginsh
ginsh_SOURCES = ginsh_parser.yy ginsh_lexer.ll ginsh.h
ginsh_LDADD = @LIBOBJS@ ../ginac/libginac.la
man_MANS = ginsh.1
YFLAGS = -d
