dnl Process this file with autoconf to produce a configure script.

dnl GiNaC version number
m4_define([ginac_major_version], [1])
m4_define([ginac_minor_version], [6])
m4_define([ginac_micro_version], [2])
m4_define([ginac_version], [ginac_major_version.ginac_minor_version.ginac_micro_version])
m4_define([ginac_release], [ginac_major_version.ginac_minor_version])

dnl GiNaC library version information. It has very little to do with GiNaC
dnl version number. In particular, library version is OS dependent. 
dnl
dnl When making releases, do
dnl 1. Increment ginac_lt_revision
dnl 2. If any interfaces have been added, removed, or changed since the last
dnl    release, increment ginac_lt_current and set ginac_lt_revision to 0.
dnl 3. If any interfaces have been added since the last release, increment
dnl    ginac_lt_age.
dnl 4. If any interfaces have been removed since the last release, set 
dnl    ginac_lt_age to 0.
dnl
dnl Please note: the libtool naming scheme cannot guarantee that on all
dnl systems, the numbering is consecutive. It only guarantees that it is
dnl increasing. This doesn't matter, though: there is not incurred cost
dnl for numbers that are omitted, except for shrinking the available space
dnl of leftover numbers. Not something we need to worry about yet. ;-)
m4_define([ginac_lt_current], [3])
m4_define([ginac_lt_revision], [0])
m4_define([ginac_lt_age], [1])

AC_INIT([GiNaC], ginac_version, [ginac-list@ginac.de], [ginac], [http://www.ginac.de/])
AC_PREREQ(2.59)
AC_CONFIG_SRCDIR(ginac/basic.cpp)
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_HEADERS([config/config.h ginac/version.h])
AC_CONFIG_MACRO_DIR([m4])
dnl This defines PACKAGE and VERSION.
AM_INIT_AUTOMAKE([gnu 1.8 dist-bzip2])

dnl Process GiNaC version information
AC_DEFINE([GINACLIB_MAJOR_VERSION], ginac_major_version, [Major version of GiNaC])
AC_DEFINE([GINACLIB_MINOR_VERSION], ginac_minor_version, [Minor version of GiNaC])
AC_DEFINE([GINACLIB_MICRO_VERSION], ginac_micro_version, [Micro version of GiNaC])

dnl GiNaC archive file version information.
dnl
dnl If properties have been added, ARCHIVE_VERSION += 1, ARCHIVE_AGE += 1.
dnl If backwards compatibility has been broken, set ARCHIVE_AGE to 0.
dnl
dnl The version number in newly created archives will be ARCHIVE_VERSION.
dnl Archives version (ARCHIVE_VERSION-ARCHIVE_AGE) thru ARCHIVE_VERSION can
dnl be read by this version of the GiNaC library.

ARCHIVE_VERSION=3
ARCHIVE_AGE=3

AC_SUBST(ARCHIVE_VERSION)
AC_SUBST(ARCHIVE_AGE)
AC_DEFINE_UNQUOTED(ARCHIVE_VERSION, $ARCHIVE_VERSION, [Current GiNaC archive file version number])
AC_DEFINE_UNQUOTED(ARCHIVE_AGE, $ARCHIVE_AGE, [GiNaC archive file version age])

dnl libtool versioning
LT_VERSION_INFO="ginac_lt_current:ginac_lt_revision:ginac_lt_age"
AC_SUBST(LT_VERSION_INFO)

dnl Check for the compiler and all the utilities needed for the build.
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL
AM_PROG_LIBTOOL
AC_PROG_LEX
AC_PROG_YACC
AC_PATH_PROG(YACCEXE, $YACC, "")
AS_IF([test "x$LEX" = "x:" -a ! -f $srcdir/ginsh/ginsh_lexer.cc],
      [GINAC_WARNING([ginsh will not compile, because lex/flex is missing])])
AS_IF([test -z "$YACCEXE" -a ! -f $srcdir/ginsh/ginsh_parser.h],
      [GINAC_WARNING([ginsh will not compile, because yacc/bison is missing])])

dnl Switch to C++ language mode for the following libraries and headers.
AC_LANG([C++])

dnl Make sure all the necessary standard headers are installed on the system.
GINAC_STD_CXX_HEADERS

dnl We need to have CLN installed.
PKG_CHECK_MODULES(CLN, cln >= 1.2.2)
AC_LIB_LINKFLAGS_FROM_LIBS([CLN_RPATH], [$CLN_LIBS], [1])
LIBS="$LIBS $CLN_LIBS $CLN_RPATH"
CPPFLAGS="$CPPFLAGS $CLN_CFLAGS"
dnl Include proper rpath into pkg-config meta data, so
dnl g++ `pkg-config --cflags --libs ginac`
dnl actually works
GINACLIB_LIBS='-L${libdir} -lginac'
AC_LIB_LINKFLAGS_FROM_LIBS([GINACLIB_RPATH], [$GINACLIB_LIBS])

dnl Check for data types which are needed by the hash function 
dnl (golden_ratio_hash).
AC_CHECK_TYPE(long long)

dnl Check for stuff needed for building the GiNaC interactive shell (ginsh).
AC_CHECK_HEADERS(unistd.h)
GINAC_HAVE_RUSAGE
GINAC_READLINE

dnl Check for dl library (needed for GiNaC::compile).
GINAC_EXCOMPILER
AM_CONDITIONAL(CONFIG_EXCOMPILER, [test "x${CONFIG_EXCOMPILER}" = "xyes"])

dnl Check for utilities needed by the different kinds of documentation.
dnl Documentation needs only be built when extending it, so never mind if it
dnl cannot find those helpers:
AC_PATH_PROG(DOXYGEN, doxygen, "")
AM_CONDITIONAL(CONFIG_DOXYGEN, [test ! -z "$DOXYGEN"])
AC_PATH_PROG(LATEX, latex, "")
AC_PATH_PROG(PDFLATEX, pdflatex, "")
AC_PATH_PROG(MAKEINDEX, makeindex, "")
AC_PATH_PROG(MAKEINFO, makeinfo, "")
AC_PATH_PROG(DVIPS, dvips, "")
AM_CONDITIONAL(CONFIG_TEX, [test ! \( -z "$LATEX" -o -z $"PDFLATEX" -o -z "$MAKEINDEX" -o -z "$DVIPS" \)])
AC_PATH_PROG(FIG2DEV, fig2dev, "")
AM_CONDITIONAL(CONFIG_FIG2DEV, [test ! -z "$FIG2DEV"])
AS_IF([test -z "$FIG2DEV" -o -z "$MAKEINFO"],
	  [GINAC_WARNING([Documentation may not build, because fig2dev/texinfo is missing.])])
AS_IF([test -z "$DOXYGEN"],
	  [GINAC_WARNING([Reference manual will not build, because doxygen is missing.])])
AS_IF([test -z "$LATEX" -o -z "$PDFLATEX" -o -z "$DVIPS"],
	  [GINAC_WARNING([Some documentation formats may not build, because latex/pdflatex/dvips is missing.])])
	   
dnl Output makefiles etc.
AC_CONFIG_FILES([
Makefile
GiNaC.spec
ginac.pc
ginac/Makefile
check/Makefile
ginsh/Makefile
ginsh/ginsh.1
tools/Makefile
tools/viewgar.1
tools/ginac-excompiler
doc/Makefile
doc/examples/Makefile
doc/tutorial/Makefile
doc/reference/Makefile
doc/reference/DoxyfileHTML
doc/reference/DoxyfileTEX
doc/reference/DoxyfilePDF
doc/reference/Doxyfooter
])
AC_OUTPUT

dnl Display a final warning if there has been a GINAC_ERROR or a GINAC_WARNING
GINAC_CHECK_ERRORS
