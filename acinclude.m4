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
        AC_MSG_ERROR([You either need to set \$LDFLAGS or update CLN])
    ;;
dnl should never ever get here:
    *)
        LIBS="${saved_LIBS}"
        AC_MSG_WARN([you found a bug in the configure script!])
    ;;
    esac
])
