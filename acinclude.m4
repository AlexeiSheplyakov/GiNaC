dnl ===========================================================================
dnl Additional macros used to configure GiNaC.  We don't start our own 
dnl additions' names with AC_ but with GINAC_ in order to steer clear of
dnl future trouble.
dnl ===========================================================================

dnl Generally, it is a bad idea to put specialized header files for a library
dnl into a generic directory like /usr/local/include/.  Instead, one should put
dnl them into a subdirectory.  GiNaC does it, NTL does it.  Unfortunately, CLN
dnl doesn't do so but some people choose to do it by hand.  In these cases we
dnl need to #include <cln/cln.h>, otherwise #include <cln.h>.  This macro
dnl tries to be clever and find out the correct way by defining the variable
dnl HAVE_CLN_CLN_H in config.h:
AC_DEFUN(GINAC_CHECK_CLN_H,
    [AC_PROVIDE([$0])
    AC_CHECK_HEADERS(cln/cln.h, ,
        AC_CHECK_HEADERS(cln.h, ,
            AC_MSG_ERROR([cannot find header for Bruno Haible's CLN]);
        )
    )
])

dnl This macro is needed because the generic AC_CHECK_LIB doesn't work because
dnl C++ is more strongly typed than C.  Therefore we need to work with the 
dnl more fundamental AC_TRY_LINK instead.
AC_DEFUN(GINAC_CHECK_LIBCLN,
    [AC_PROVIDE([$0])
    AC_MSG_CHECKING([for doublefactorial in -lcln])
    saved_LIBS="${LIBS}"
    AC_CACHE_VAL(ginac_cv_lib_cln_link,
        [LIBS="-lcln"
        case "${ac_cv_header_cln_cln_h}" in
        "yes")
            AC_TRY_LINK([#include <cln/cln.h>],
                [doublefactorial(2);],
                ginac_cv_lib_cln_link="-lcln",
                ginac_cv_lib_cln_link="fail")
            ;;
        *)
            AC_TRY_LINK([#include <cln.h>],
                [doublefactorial(2);],
                ginac_cv_lib_cln_link="-lcln",
                ginac_cv_lib_cln_link="fail")
            ;;
        esac
    ])
    case "${ginac_cv_lib_cln_link}" in
dnl linking worked:
    "-lcln")
        LIBS="${ginac_cv_lib_cln_link} ${saved_LIBS}"
        AC_MSG_RESULT("yes")
    ;;
dnl linking failed:
    "fail")
        LIBS="${saved_LIBS}"
        AC_MSG_RESULT([no])
        GINAC_ERROR([I could not successfully link a test-program against libcln and run it.
   You either need to set \$LDFLAGS or install/update the CLN library.])
    ;;
    esac
])

dnl Usage: GINAC_ERROR(message)
dnl This macro displays the warning "message" and sets the flag ginac_error
dnl to yes.
AC_DEFUN(GINAC_ERROR,[
ginac_error_txt="$ginac_error_txt
** $1
"
ginac_error=yes])

dnl Usage: GINAC_WARNING(message)
dnl This macro displays the warning "message" and sets the flag ginac_warning
dnl to yes.
AC_DEFUN(GINAC_WARNING,[
ginac_warning_txt="$ginac_warning_txt
== $1
"
ginac_warning=yes])

dnl Usage: GINAC_CHECK_ERRORS
dnl (preferably to be put at end of configure.in)
dnl This macro displays a warning message if GINAC_ERROR or GINAC_WARNING 
dnl has occured previously.
AC_DEFUN(GINAC_CHECK_ERRORS,[
if test "x${ginac_error}" = "xyes"; then
  echo "**** The following problems have been detected by configure."
  echo "**** Please check the messages below before running \"make\"."
  echo "**** (see the section 'Common Problems' in the INSTALL file)"
  echo "$ginac_error_txt"
  if test "x${ginac_warning_txt}" != "x"; then
    echo "${ginac_warning_txt}"
  fi
  echo "deleting cache ${cache_file}"
  rm -f $cache_file
  else 
    if test x$ginac_warning = xyes; then
      echo "=== The following minor problems have been detected by configure."
      echo "=== Please check the messages below before running \"make\"."
      echo "=== (see the section 'Common Problems' in the INSTALL file)"
      echo "$ginac_warning_txt"
    fi
  echo "Configuration of GiNaC $VERSION done. Now type \"make\"."
fi])
