dnl Process this file with autoconf to produce a configure script.

dnl GiNaC library version information.
dnl
dnl Making releases:
dnl   ginac_micro_version += 1;
dnl   ginac_interface_age += 1;
dnl   ginac_binary_age += 1;
dnl if any functions have been added, set ginac_interface_age to 0.
dnl if backwards compatibility has been broken,
dnl set ginac_binary_age and ginac_interface_age to 0.

m4_define([ginac_major_version], [1])
m4_define([ginac_minor_version], [5])
m4_define([ginac_micro_version], [0])
m4_define([ginac_version], [ginac_major_version.ginac_minor_version.ginac_micro_version])
m4_define([ginac_release], [ginac_major_version.ginac_minor_version])
m4_define([ginac_interface_age], [0])
m4_define([ginac_binary_age], [0])

AC_INIT([GiNaC], ginac_version, [<ginac-list@ginac.de>])
AC_PREREQ(2.59)
AC_CONFIG_SRCDIR(ginac/basic.cpp)
AC_CONFIG_HEADERS(config.h)
dnl This defines PACKAGE and VERSION.
AM_INIT_AUTOMAKE([gnu 1.7 dist-bzip2])

dnl Process GiNaC version information
GINACLIB_MAJOR_VERSION=ginac_major_version
GINACLIB_MINOR_VERSION=ginac_minor_version
GINACLIB_MICRO_VERSION=ginac_micro_version
GINACLIB_VERSION=ginac_version
AC_SUBST(GINACLIB_MAJOR_VERSION)
AC_SUBST(GINACLIB_MINOR_VERSION)
AC_SUBST(GINACLIB_MICRO_VERSION)
AC_SUBST(GINACLIB_VERSION)

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
m4_define([lt_revision], [ginac_interface_age])
m4_define([lt_current], [m4_eval(ginac_micro_version - ginac_interface_age)])
m4_define([lt_age], [m4_eval(ginac_binary_age - ginac_interface_age)])
LT_VERSION_INFO="lt_current:lt_revision:lt_age"
LT_RELEASE="ginac_release"

AC_SUBST(LT_VERSION_INFO)
AC_SUBST(LT_RELEASE)


dnl Check for the compiler and all the utilities needed for the build.
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL
AM_PROG_LIBTOOL
AM_PROG_LEX
AC_PROG_YACC

dnl Check for data types which are needed by the hash function 
dnl (golden_ratio_hash).
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)
AC_CHECK_SIZEOF(void *)

dnl Switch to C++ language mode for the following libraries and headers.
AC_LANG([C++])

dnl Make sure all the necessary standard headers are installed on the system.
GINAC_STD_CXX_HEADERS

dnl Check for stuff needed for building the GiNaC interactive shell (ginsh).
AC_CHECK_HEADERS(unistd.h)
GINAC_HAVE_RUSAGE
GINAC_READLINE

DL_LIBS=""
dnl Check for dl library (needed for GiNaC::compile).
AC_CHECK_LIB(dl, dlopen,
	[
		DL_LIBS="-ldl"
		AC_DEFINE(HAVE_LIBDL, 1, [set to 1 if you have a working libdl installed.])
	], 
	GINAC_WARNING([libdl not found. GiNaC::compile will be disabled.]))
AC_SUBST(DL_LIBS)

dnl We need to have Bruno Haible's CLN installed.
dnl (pkg-config must have installed pkg.m4 at a visible place, which provides
dnl this macro. CLN >= 1.1.6 must have installed cln.pc at a visible place,
dnl which provides the actual dependency information.)
PKG_CHECK_MODULES(CLN, cln >= 1.1.6)
LIBS="$LIBS $CLN_LIBS"
CPPFLAGS="$CPPFLAGS $CLN_CFLAGS"

dnl Check for utilities needed by the different kinds of documentation.
dnl Documentation needs only be built when extending it, so never mind if it
dnl cannot find those helpers:
AC_PATH_PROG(DOXYGEN, doxygen, "")
AM_CONDITIONAL(CONFIG_DOXYGEN, [test ! -z "$DOXYGEN"])
AC_PATH_PROG(LATEX, latex, "")
AC_PATH_PROG(PDFLATEX, pdflatex, "")
AC_PATH_PROG(MAKEINDEX, makeindex, "")
AC_PATH_PROG(DVIPS, dvips, "")
AM_CONDITIONAL(CONFIG_TEX, [test ! \( -z "$LATEX" -o -z $"PDFLATEX" -o -z "$MAKEINDEX" -o -z "$DVIPS" \)])
AC_PATH_PROG(FIG2DEV, fig2dev, "")
AM_CONDITIONAL(CONFIG_FIG2DEV, [test ! -z "$FIG2DEV"])

dnl generate boilerplate code for the (new) parser.
dnl Only developers need this tool.
AC_PATH_PROG(AUTOGEN, autogen, "")

dnl Output makefiles etc.
AC_CONFIG_FILES([
Makefile
GiNaC.spec
ginac.pc
ginac/Makefile
ginac/version.h
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
