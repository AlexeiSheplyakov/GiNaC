## Process this file with automake to produce Makefile.in

bin_PROGRAMS = ginsh
ginsh_SOURCES = ginsh_parser.yy ginsh_lexer.ll ginsh.h
ginsh_LDADD = ../ginac/libginac.la $(GINSH_LIBS)
man_MANS = ginsh.1
YFLAGS = -d
EXTRA_DIST = $(man_MANS) ginsh_parser.h ginsh_fcn_help.sed ginsh_op_help.sed

# files created by sed scripts
ginsh_fcn_help.c: ginsh.1 ginsh_fcn_help.sed
	sed -n -f ginsh_fcn_help.sed <$< >$@

ginsh_op_help.c: ginsh.1 ginsh_op_help.sed
	sed -n -f ginsh_op_help.sed <$< >$@

ginsh_parser.o: ginsh_fcn_help.c ginsh_op_help.c
