dnl ===========================================================================
dnl Additional macros used to configure GiNaC.  We don't start our own 
dnl additions' names with AC_ but with GINAC_ in order to steer clear of
dnl future trouble.
dnl ===========================================================================

dnl Usage: GINAC_RLVERSION
dnl The maintainers of libreadline are complete morons: they don't care a shit
dnl about compatiblilty (which is not so bad by itself) and at the same time 
dnl they don't export the version to the preprocessor so we could kluge around 
dnl incomatiblities.  The only reliable way to figure out the version is by 
dnl checking the extern variable rl_library_version at runtime.  &#@$%*!
AC_DEFUN([GINAC_LIB_READLINE_VERSION],
[AC_CACHE_CHECK([for version of libreadline], ginac_cv_rlversion, [
AC_TRY_RUN([
#include <stdio.h>
#include <sys/types.h>
#include <readline/readline.h>

int main()
{
    FILE *fd;
    fd = fopen("conftest.out", "w");
    fprintf(fd, "%s\n", rl_library_version);
    fclose(fd);
    return 0;
}], [
dnl Some non-GNU readline implementations have non-numeric rl_library_version
ginac_cv_rlversion=`sed -e 's/[[^0-9.]]//g' 'conftest.out'`], [ ginac_cv_rlversion='unknown'], [ ginac_cv_rlversion='4.2'])])
if test -z "$ginac_cv_rlversion"; then
	GINAC_WARNING([Unsupported version of libreadline.])
	ginac_cv_rlversion='unknown'
fi
if test "x${ginac_cv_rlversion}" != "xunknown"; then
	AC_DEFINE(REALLY_HAVE_LIBREADLINE, ,[Define if GNU libreadline is installed])
  RL_VERSION_MAJOR=`echo ${ginac_cv_rlversion} | sed -e 's/\([[0-9]]*\)\.\([[0-9]]*\).*/\1/'`
  AC_DEFINE_UNQUOTED(GINAC_RL_VERSION_MAJOR, $RL_VERSION_MAJOR, [Major version of installed readline library.])
  RL_VERSION_MINOR=`echo ${ginac_cv_rlversion} | sed -e 's/\([[0-9]]*\)\.\([[0-9]]*\).*/\2/'`
  AC_DEFINE_UNQUOTED(GINAC_RL_VERSION_MINOR, $RL_VERSION_MINOR, [Minor version of installed readline library.])
else
  GINAC_WARNING([I could not run a test of libreadline (needed for building ginsh).])
fi
])

dnl Usage: GINAC_TERMCAP
dnl libreadline is based on the termcap functions.
dnl Some systems have tgetent(), tgetnum(), tgetstr(), tgetflag(), tputs(),
dnl tgoto() in libc, some have it in libtermcap, some have it in libncurses.
dnl When both libtermcap and libncurses exist, we prefer the latter, because
dnl libtermcap is being phased out.
AC_DEFUN([GINAC_TERMCAP],
[LIBTERMCAP=
case $host_os in
*mingw32*)
 ;; dnl no termcap libraries are necessary (need hacked libreadline)
*)
AC_CHECK_FUNCS(tgetent)
if test "x$ac_cv_func_tgetent" = "xyes"; then
    :
else
    AC_CHECK_LIB(ncurses, tgetent, LIBTERMCAP="-lncurses")
    if test -z "$LIBTERMCAP"; then
        AC_CHECK_LIB(termcap, tgetent, LIBTERMCAP="-ltermcap")
    fi
fi
;;
esac
AC_SUBST(LIBTERMCAP)
])

dnl Usage: GINAC_ERROR(message)
dnl This macro displays the warning "message" and sets the flag ginac_error
dnl to yes.
AC_DEFUN([GINAC_ERROR],[
ginac_error_txt="$ginac_error_txt
** $1
"
ginac_error=yes])

dnl Usage: GINAC_WARNING(message)
dnl This macro displays the warning "message" and sets the flag ginac_warning
dnl to yes.
AC_DEFUN([GINAC_WARNING],[
ginac_warning_txt="$ginac_warning_txt
== $1
"
ginac_warning=yes])

dnl Usage: GINAC_CHECK_ERRORS
dnl (must be put at end of configure.in, because it exits on error)
dnl This macro displays a warning message if GINAC_ERROR or GINAC_WARNING 
dnl has occured previously.
AC_DEFUN([GINAC_CHECK_ERRORS],[
if test "x${ginac_error}" = "xyes"; then
    echo "**** The following problems have been detected by configure."
    echo "**** Please check the messages below before running \"make\"."
    echo "**** (see the section 'Common Problems' in the INSTALL file)"
    echo "$ginac_error_txt"
    if test "x${ginac_warning_txt}" != "x"; then
        echo "${ginac_warning_txt}"
    fi
    if test "x$cache_file" != "x/dev/null"; then
        echo "deleting cache ${cache_file}"
        rm -f $cache_file
    fi
    exit 1
else 
    if test "x${ginac_warning}" = "xyes"; then
        echo "=== The following minor problems have been detected by configure."
        echo "=== Please check the messages below before running \"make\"."
        echo "=== (see the section 'Common Problems' in the INSTALL file)"
        echo "$ginac_warning_txt"
    fi
    echo "Configuration of GiNaC $VERSION done. Now type \"make\"."
fi])

AC_DEFUN([GINAC_HAVE_RUSAGE],
[AC_CACHE_CHECK([whether struct rusage is declared in <sys/resource.h>],
ac_cv_have_rusage,
 [AC_TRY_COMPILE([#include <sys/times.h>
                  #include <sys/resource.h>],
                  [struct rusage resUsage;
                   getrusage(RUSAGE_SELF, &resUsage);
                   return 0;],
                 [ac_cv_have_rusage=yes],
                 [ac_cv_have_rusage=no])
])
CONFIG_RUSAGE="no"
if test "$ac_cv_have_rusage" = yes; then
  CONFIG_RUSAGE="yes"
  AC_DEFINE(HAVE_RUSAGE,,[define if struct rusage declared in <sys/resource.h>])
fi
AC_SUBST(CONFIG_RUSAGE)
])
