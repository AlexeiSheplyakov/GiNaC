# Configure paths for GiNaC
# Richard Kreckel 12/12/2000
# borrowed from Christian Bauer
# stolen from Sam Lantinga
# stolen from Manish Singh
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl AM_PATH_GINAC([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for GiNaC, and define GINACLIB_CPPFLAGS and GINACLIB_LIBS
dnl
AC_DEFUN([AM_PATH_GINAC],
[dnl 
dnl Get the cppflags and libraries from the ginac-config script
dnl
AC_ARG_WITH(ginac-prefix, AC_HELP_STRING([--with-ginac-prefix=PFX], [Prefix where GiNaC is installed (optional)]),
            ginac_prefix="$withval", ginac_prefix="")
AC_ARG_WITH(ginac-exec-prefix, AC_HELP_STRING([--with-ginac-exec-prefix=PFX], [Exec prefix where GiNaC is installed (optional)]),
            ginac_exec_prefix="$withval", ginac_exec_prefix="")
AC_ARG_ENABLE(ginactest, AC_HELP_STRING([--disable-ginactest], [Do not try to compile and run a test GiNaC program]),
              , enable_ginactest=yes)

if test x$ginac_exec_prefix != x ; then
    ginac_args="$ginac_args --exec-prefix=$ginac_exec_prefix"
    if test x${GINACLIB_CONFIG+set} != xset ; then
        GINACLIB_CONFIG=$ginac_exec_prefix/bin/ginac-config
    fi
fi
if test x$ginac_prefix != x ; then
    ginac_args="$ginac_args --prefix=$ginac_prefix"
    if test x${GINACLIB_CONFIG+set} != xset ; then
        GINACLIB_CONFIG=$ginac_prefix/bin/ginac-config
    fi
fi

AC_PATH_PROG(GINACLIB_CONFIG, ginac-config, no)
ginac_min_version=ifelse([$1], ,0.7.0,$1)
AC_MSG_CHECKING(for GiNaC - version >= $ginac_min_version)
if test "$GINACLIB_CONFIG" = "no" ; then
    AC_MSG_RESULT(no)
    echo "*** The ginac-config script installed by GiNaC could not be found"
    echo "*** If GiNaC was installed in PREFIX, make sure PREFIX/bin is in"
    echo "*** your path, or set the GINACLIB_CONFIG environment variable to the"
    echo "*** full path to ginac-config."
    ifelse([$3], , :, [$3])
else
dnl Parse required version and the result of ginac-config.
    ginac_min_major_version=`echo $ginac_min_version | \
            sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    ginac_min_minor_version=`echo $ginac_min_version | \
            sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    ginac_min_micro_version=`echo $ginac_min_version | \
            sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    GINACLIB_CPPFLAGS=`$GINACLIB_CONFIG $ginac_args --cppflags`
    GINACLIB_LIBS=`$GINACLIB_CONFIG $ginac_args --libs`
    ginac_config_version=`$GINACLIB_CONFIG $ginac_args --version`
    ginac_config_major_version=`echo $ginac_config_version | \
            sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    ginac_config_minor_version=`echo $ginac_config_version | \
            sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    ginac_config_micro_version=`echo $ginac_config_version | \
            sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
dnl Check if the installed GiNaC is sufficiently new according to ginac-config.
    if test \( $ginac_config_major_version -lt $ginac_min_major_version \) -o \
            \( $ginac_config_major_version -eq $ginac_min_major_version -a $ginac_config_minor_version -lt $ginac_min_minor_version \) -o \
            \( $ginac_config_major_version -eq $ginac_min_major_version -a $ginac_config_minor_version -eq $ginac_min_minor_version -a $ginac_config_micro_version -lt $ginac_min_micro_version \); then
        echo -e "\n*** 'ginac-config --version' returned $ginac_config_major_version.$ginac_config_minor_version.$ginac_config_micro_version, but the minimum version"
        echo "*** of GiNaC required is $ginac_min_major_version.$ginac_min_minor_version.$ginac_min_micro_version. If ginac-config is correct, then it is"
        echo "*** best to upgrade to the required version."
        echo "*** If ginac-config was wrong, set the environment variable GINACLIB_CONFIG"
        echo "*** to point to the correct copy of ginac-config, and remove the file"
        echo "*** config.cache before re-running configure."
        ifelse([$3], , :, [$3])
    else
dnl The versions match so far.  Now do a sanity check: Does the result of ginac-config
dnl match the version of the headers and the version built into the library, too?
        no_ginac=""
        if test "x$enable_ginactest" = "xyes" ; then
            ac_save_CPPFLAGS="$CPPFLAGS"
            ac_save_LIBS="$LIBS"
            CPPFLAGS="$CPPFLAGS $GINACLIB_CPPFLAGS"
            LIBS="$LIBS $GINACLIB_LIBS"
            rm -f conf.ginactest
            AC_TRY_RUN([
#include <stdio.h>
#include <string.h>
#include <ginac/version.h>

/* we do not #include <stdlib.h> because autoconf in C++ mode inserts a
   prototype for exit() that conflicts with the one in stdlib.h */
extern "C" int system(const char *);

int main()
{
    int major, minor, micro;
    char *tmp_version;

    system("touch conf.ginactest");

    if ((GINACLIB_MAJOR_VERSION != $ginac_config_major_version) ||
        (GINACLIB_MINOR_VERSION != $ginac_config_minor_version) ||
        (GINACLIB_MICRO_VERSION != $ginac_config_micro_version)) {
        printf("\n*** 'ginac-config --version' returned %d.%d.%d, but the header file I found\n", $ginac_config_major_version, $ginac_config_minor_version, $ginac_config_micro_version);
        printf("*** corresponds to %d.%d.%d. This mismatch suggests your installation of GiNaC\n", GINACLIB_MAJOR_VERSION, GINACLIB_MINOR_VERSION, GINACLIB_MICRO_VERSION);
        printf("*** is corrupted or you have specified some wrong -I compiler flags.\n");
        printf("*** Please inquire and consider reinstalling GiNaC.\n");
        return 1;
    }
    if ((GiNaC::version_major != $ginac_config_major_version) ||
        (GiNaC::version_minor != $ginac_config_minor_version) ||
        (GiNaC::version_micro != $ginac_config_micro_version)) {
        printf("\n*** 'ginac-config --version' returned %d.%d.%d, but the library I found\n", $ginac_config_major_version, $ginac_config_minor_version, $ginac_config_micro_version);
        printf("*** corresponds to %d.%d.%d. This mismatch suggests your installation of GiNaC\n", GiNaC::version_major, GiNaC::version_minor, GiNaC::version_micro);
        printf("*** is corrupted or you have specified some wrong -L compiler flags.\n");
        printf("*** Please inquire and consider reinstalling GiNaC.\n");
        return 1;
    }
    return 0;
}
],, no_ginac=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
            CPPFLAGS="$ac_save_CPPFLAGS"
            LIBS="$ac_save_LIBS"
        fi
        if test "x$no_ginac" = x ; then
            AC_MSG_RESULT([yes, $ginac_config_version])
            ifelse([$2], , :, [$2])
        else
            AC_MSG_RESULT(no)
            if test ! -f conf.ginactest ; then
                echo "*** Could not run GiNaC test program, checking why..."
                CPPFLAGS="$CFLAGS $GINACLIB_CPPFLAGS"
                LIBS="$LIBS $GINACLIB_LIBS"
                AC_TRY_LINK([
#include <stdio.h>
#include <ginac/version.h>
],              [ return 0; ],
                [ echo "*** The test program compiled, but did not run. This usually means"
                  echo "*** that the run-time linker is not finding GiNaC or finding the wrong"
                  echo "*** version of GiNaC. If it is not finding GiNaC, you'll need to set your"
                  echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
                  echo "*** to the installed location. Also, make sure you have run ldconfig if that"
                  echo "*** is required on your system."],
                [ echo "*** The test program failed to compile or link. See the file config.log for the"
                  echo "*** exact error that occured. This usually means GiNaC was incorrectly installed"
                  echo "*** or that you have moved GiNaC since it was installed. In the latter case, you"
                  echo "*** may want to edit the ginac-config script: $GINACLIB_CONFIG." ])
                CPPFLAGS="$ac_save_CPPFLAGS"
                LIBS="$ac_save_LIBS"
            fi
            GINACLIB_CPPFLAGS=""
            GINACLIB_LIBS=""
            ifelse([$3], , :, [$3])
        fi
    fi
fi
AC_SUBST(GINACLIB_CPPFLAGS)
AC_SUBST(GINACLIB_LIBS)
rm -f conf.ginactest
])
