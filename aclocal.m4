dnl ===========================================================================
dnl Additional macros used to configure GiNaC.  We don't start additions' names
dnl with AC_ but with GINAC_ in order to avoid future trouble.
dnl ===========================================================================

dnl Generally, it is a bad idea to put specialized header files for a library
dnl into a generic directory like /usr/local/include/.  Instead, one should put
dnl them into a subdirectory.  GiNaC does it, NTL does it.  Unfortunately, CLN
dnl doesn't do so but some people choose to do it by hand.  In these cases we
dnl need to #include <CLN/cln.h>, otherwise #include <cln.h>.  This macro
dnl tries to be clever and find out the correct way by defining the variable
dnl HAVE_CLN_CLN_H in config.h:
AC_DEFUN(GINAC_CHECK_CLN_H,
    [AC_PROVIDE([$0])
    AC_CHECK_HEADERS(CLN/cln.h, ,
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
    AC_MSG_CHECKING([how to link with libcln])
    saved_LIBS="${LIBS}"
    AC_CACHE_VAL(ginac_cv_lib_cln_link,
        [LIBS="-lcln"
        case "${ac_cv_header_CLN_cln_h}" in
        "yes")
            AC_TRY_LINK([#include <CLN/cln.h>],
                [factorial(1);],
                ginac_cv_lib_cln_link="-lcln",
                ginac_cv_lib_cln_link="fail")
            ;;
        *)
            AC_TRY_LINK([#include <cln.h>],
                [factorial(1);],
                ginac_cv_lib_cln_link="-lcln",
                ginac_cv_lib_cln_link="fail")
            ;;
        esac
    ])
    case "${ginac_cv_lib_cln_link}" in
dnl linking worked:
    "-lcln")
        LIBS="-lcln ${saved_LIBS}"
        AC_MSG_RESULT([-lcln])
        GINAC_CHECK_LIBCLN_SANITY
    ;;
dnl linking failed:
    "fail")
        LIBS="${saved_LIBS}"
        AC_MSG_RESULT([])
        AC_MSG_WARN([linking with libcln failed])
    ;;
dnl should never ever get here:
    *)
        LIBS="${saved_LIBS}"
    ;;
    esac
])

dnl Check if the CLN library suits our needs, i.e. if it is new enough, by
dnl trying to run into a little bug which was present till version 1.0.1 and
dnl then removed.
AC_DEFUN(GINAC_CHECK_LIBCLN_SANITY,
    [AC_PROVIDE([$0])
    AC_MSG_CHECKING([whether libcln behaves sane])
    AC_CACHE_VAL(ginac_cv_lib_cln_integrity,
        [
        case "${ac_cv_header_CLN_cln_h}" in
        "yes")
            AC_TRY_RUN([#include <CLN/cln.h>
int main() {
cl_RA q(3); q = q/2; cl_RA p(3); p = p/2;
if (q+p != 3) return 1; else return 0;
}],
                ginac_cv_lib_cln_integrity="sane",
                ginac_cv_lib_cln_integrity="insane",
                ginac_cv_lib_cln_integrity="guessing sane")
            ;;
        *)
            AC_TRY_RUN([#include <cln.h>
int main() {
cl_RA q(3); q = q/2; cl_RA p(3); p = p/2;
if (q+p != 3) return 1; else return 0;
}],
                ginac_cv_lib_cln_integrity="sane",
                ginac_cv_lib_cln_integrity="insane",
                ginac_cv_lib_cln_integrity="guessing sane")
            ;;
        esac
    ])
    case "${ginac_cv_lib_cln_integrity}" in
dnl exit status was 0:
    "sane")
        AC_MSG_RESULT([yes])
    ;;
dnl exit status was not 0:
    "insane")
        AC_MSG_RESULT([no])
        AC_MSG_WARN([maybe version of libcln is older than 1.0.2?])
    ;;
dnl test-program was not run because we are cross-compiling:
    "guessing sane")
        AC_MSG_RESULT([hopefully])
    ;;
dnl should never ever get here:
    *)
        AC_MSG_WARN([you found a bug in the configure script!])
    ;;
    esac
])
