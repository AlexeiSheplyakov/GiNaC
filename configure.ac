dnl Process this file with autoconf to produce a configure script.

AC_INIT
AC_CONFIG_SRCDIR(ginac/basic.cpp)
AC_PREREQ(2.50)

dnl GiNaC library version information.
dnl
dnl Making releases:
dnl   GINACLIB_MICRO_VERSION += 1;
dnl   GINACLIB_INTERFACE_AGE += 1;
dnl   GINACLIB_BINARY_AGE += 1;
dnl if any functions have been added, set GINACLIB_INTERFACE_AGE to 0.
dnl if backwards compatibility has been broken,
dnl set GINACLIB_BINARY_AGE and GINACLIB_INTERFACE_AGE to 0.
dnl
dnl NOTE: these can't be renamed to GINAC_MAJOR_VERSION etc. because
dnl autoconf sees "AC_MAJOR_VERSION" and complains about an undefined macro
dnl (don't we all *love* M4?)...

GINACLIB_MAJOR_VERSION=1
GINACLIB_MINOR_VERSION=0
GINACLIB_MICRO_VERSION=5
GINACLIB_INTERFACE_AGE=3
GINACLIB_BINARY_AGE=5
GINACLIB_VERSION=$GINACLIB_MAJOR_VERSION.$GINACLIB_MINOR_VERSION.$GINACLIB_MICRO_VERSION

AC_SUBST(GINACLIB_MAJOR_VERSION)
AC_SUBST(GINACLIB_MINOR_VERSION)
AC_SUBST(GINACLIB_MICRO_VERSION)
AC_SUBST(GINACLIB_INTERFACE_AGE)
AC_SUBST(GINACLIB_BINARY_AGE)
AC_SUBST(GINACLIB_VERSION)

dnl GiNaC archive file version information.
dnl
dnl If properties have been added, ARCHIVE_VERSION += 1, ARCHIVE_AGE += 1.
dnl If backwards compatibility has been broken, set ARCHIVE_AGE to 0.
dnl
dnl The version number in newly created archives will be ARCHIVE_VERSION.
dnl Archives version (ARCHIVE_VERSION-ARCHIVE_AGE) thru ARCHIVE_VERSION can
dnl be read by this version of the GiNaC library.

ARCHIVE_VERSION=1
ARCHIVE_AGE=1

AC_SUBST(ARCHIVE_VERSION)
AC_SUBST(ARCHIVE_AGE)
AC_DEFINE_UNQUOTED(ARCHIVE_VERSION, $ARCHIVE_VERSION, [Current GiNaC archive file version number])
AC_DEFINE_UNQUOTED(ARCHIVE_AGE, $ARCHIVE_AGE, [GiNaC archive file version age])

dnl libtool versioning
LT_RELEASE=$GINACLIB_MAJOR_VERSION.$GINACLIB_MINOR_VERSION
LT_CURRENT=`expr $GINACLIB_MICRO_VERSION - $GINACLIB_INTERFACE_AGE`
LT_REVISION=$GINACLIB_INTERFACE_AGE
LT_AGE=`expr $GINACLIB_BINARY_AGE - $GINACLIB_INTERFACE_AGE`

AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

dnl This defines PACKAGE and VERSION.
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE([GiNaC], $GINACLIB_VERSION)

dnl Check for the compiler and all the utilities needed for the build.
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL
AM_PROG_LIBTOOL
AM_PROG_LEX
AC_PROG_YACC

dnl Configure options.
AC_ARG_ENABLE(html-doc, [  --enable-html-doc       build HTML documentation [default=no]], , enable_html_doc=no)
AC_ARG_ENABLE(ps-doc,   [  --enable-ps-doc         build PostScript documentation [default=no]], , enable_ps_doc=no)

dnl Check for data types which are needed by the hash function 
dnl (golden_ratio_hash).
AC_CHECK_SIZEOF(long, 4)
AC_CHECK_SIZEOF(long long, 8)
AC_CHECK_SIZEOF(long double, 12)

dnl Switch to C++ language mode for the following libraries and headers.
AC_LANG([C++])

dnl Check for stuff needed for building the GiNaC interactive shell (ginsh).
AC_CHECK_HEADERS(unistd.h)
AC_CHECK_HEADERS(readline/readline.h readline/history.h)
if test "x${ac_cv_header_readline_readline_h}" != "xyes" -o "x${ac_cv_header_readline_history_h}" != "xyes"; then
  GINAC_WARNING([I could not find the headers for libreadline (needed for building ginsh).])
fi
GINAC_TERMCAP
save_LIBS=$LIBS
LIBS="$LIBTERMCAP $LIBS"
AC_CHECK_LIB(readline, readline)
if test "x${ac_cv_lib_readline_readline}" = "xyes"; then
  GINAC_LIB_READLINE_VERSION
else
  GINAC_WARNING([I could not find libreadline (needed by ginsh).])
fi
GINSH_LIBS=$LIBS
LIBS=$save_LIBS
AC_SUBST(GINSH_LIBS)

dnl Make sure all the necessary standard headers are installed on the system.
AC_CHECK_HEADER(iosfwd, , GINAC_ERROR([The standard <iosfwd> header file could not be found.]))
AC_CHECK_HEADER(iostream, , GINAC_ERROR([The standard <iostream> header file could not be found.]))
AC_CHECK_HEADER(vector, , GINAC_ERROR([The standard <vector> header file could not be found.]))
AC_CHECK_HEADER(list, , GINAC_ERROR([The standard <list> header file could not be found.]))
AC_CHECK_HEADER(map, , GINAC_ERROR([The standard <map> header file could not be found.]))
AC_CHECK_HEADER(string, , GINAC_ERROR([The standard <string> header file could not be found.]))
AC_CHECK_HEADER(sstream, , GINAC_ERROR([The standard <sstream> header file could not be found.]))
AC_CHECK_HEADER(typeinfo, , GINAC_ERROR([The standard <typeinfo> header file could not be found.]))
AC_CHECK_HEADER(stdexcept, , GINAC_ERROR([The standard <stdexcept> header file could not be found.]))
AC_CHECK_HEADER(algorithm, , GINAC_ERROR([The standard <algorithm> header file could not be found.]))

dnl We need to have Bruno Haible's CLN installed.
dnl (CLN versions >= 1.1.0 must have installed cln.m4 at a visible place,
dnl which provides this macro):
AC_PATH_CLN(1.1.0, [
  LIBS="$LIBS $CLN_LIBS"
  CPPFLAGS="$CPPFLAGS $CLN_CPPFLAGS"
], GINAC_ERROR([No suitable installed version of CLN could be found.]))

dnl Expand the cppflags and libraries needed by apps using GiNaC
GINACLIB_CPPFLAGS=$CPPFLAGS
GINACLIB_LIBS=$LIBS
AC_SUBST(GINACLIB_CPPFLAGS)
AC_SUBST(GINACLIB_LIBS)

dnl Check for utilities needed by the different kinds of documentation.
dnl Documentation needs only be built when extending it, so never mind if it
dnl cannot find those helpers:
AC_PATH_PROG(DOXYGEN, doxygen, "")
AC_PATH_PROG(LATEX, latex, "")
AC_PATH_PROG(MAKEINDEX, makeindex, "")
AC_PATH_PROG(DVIPS, dvips, "")
AC_PATH_PROG(FIG2DEV, fig2dev, "")

dnl Determine which documentation shall be built
TUTORIAL_TARGETS=
REFERENCE_TARGETS=
if test "x${enable_html_doc}" = "xyes"; then
  TUTORIAL_TARGETS="$TUTORIAL_TARGETS ginac.html"
  if [[ "$DOXYGEN" ]]; then
    REFERENCE_TARGETS="$REFERENCE_TARGETS index.html"
  fi
fi
if test "x${enable_ps_doc}" = "xyes"; then
  TUTORIAL_TARGETS="$TUTORIAL_TARGETS ginac.ps"
  if [[ "$DOXYGEN" ]]; then
    REFERENCE_TARGETS="$REFERENCE_TARGETS reference.ps"
  fi
fi
AC_SUBST(TUTORIAL_TARGETS)
AC_SUBST(REFERENCE_TARGETS)

dnl Output makefiles etc.
AC_CONFIG_FILES([
Makefile
GiNaC.spec
ginac-config
ginac-config.1
ginac/Makefile
ginac/version.h
check/Makefile
ginsh/Makefile
ginsh/ginsh.1
tools/Makefile
tools/viewgar.1
doc/Makefile
doc/tutorial/Makefile
doc/reference/Makefile
])
AC_CONFIG_COMMANDS([default],[[chmod +x ginac-config]],[[]])
AC_OUTPUT
dnl Display a final warning if there has been a GINAC_ERROR or a GINAC_WARNING
GINAC_CHECK_ERRORS
