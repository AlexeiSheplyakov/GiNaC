# Configure paths for GiNaC
# Christian Bauer 12/1/99
# stolen from Sam Lantinga
# stolen from Manish Singh
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl AM_PATH_GINAC([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for GiNaC, and define GINACLIB_CPPFLAGS and GINACLIB_LIBS
dnl
AC_DEFUN(AM_PATH_GINAC,
[dnl 
dnl Get the cppflags and libraries from the ginac-config script
dnl
AC_ARG_WITH(ginac-prefix,[  --with-ginac-prefix=PFX Prefix where GiNaC is installed (optional)],
            ginac_prefix="$withval", ginac_prefix="")
AC_ARG_WITH(ginac-exec-prefix,[  --with-ginac-exec-prefix=PFX Exec prefix where GiNaC is installed (optional)],
            ginac_exec_prefix="$withval", ginac_exec_prefix="")
AC_ARG_ENABLE(ginactest, [  --disable-ginactest     Do not try to compile and run a test GiNaC program],
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
  min_ginac_version=ifelse([$1], ,0.4.0,$1)
  AC_MSG_CHECKING(for GiNaC - version >= $min_ginac_version)
  no_ginac=""
  if test "$GINACLIB_CONFIG" = "no" ; then
    no_ginac=yes
  else
    GINACLIB_CPPFLAGS=`$GINACLIB_CONFIG $ginacconf_args --cppflags`
    GINACLIB_LIBS=`$GINACLIB_CONFIG $ginacconf_args --libs`

    ginac_major_version=`$GINACLIB_CONFIG $ginac_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    ginac_minor_version=`$GINACLIB_CONFIG $ginac_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    ginac_micro_version=`$GINACLIB_CONFIG $ginac_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_ginactest" = "xyes" ; then
      ac_save_CPPFLAGS="$CPPFLAGS"
      ac_save_LIBS="$LIBS"
      CPPFLAGS="$CPPFLAGS $GINACLIB_CPPFLAGS"
      LIBS="$LIBS $GINACLIB_LIBS"
dnl
dnl Now check if the installed GiNaC is sufficiently new. (Also sanity
dnl checks the results of ginac-config to some extent
dnl
      rm -f conf.ginactest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ginac/ginac.h>

char*
my_strdup (char *str)
{
  char *new_str;
  
  if (str)
    {
      new_str = (char *)malloc ((strlen (str) + 1) * sizeof(char));
      strcpy (new_str, str);
    }
  else
    new_str = NULL;
  
  return new_str;
}

int main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.ginactest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_ginac_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_ginac_version");
     exit(1);
   }

   if (($ginac_major_version > major) ||
      (($ginac_major_version == major) && ($ginac_minor_version > minor)) ||
      (($ginac_major_version == major) && ($ginac_minor_version == minor) && ($ginac_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'ginac-config --version' returned %d.%d.%d, but the minimum version\n", $ginac_major_version, $ginac_minor_version, $ginac_micro_version);
      printf("*** of GiNaC required is %d.%d.%d. If ginac-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If ginac-config was wrong, set the environment variable GINACLIB_CONFIG\n");
      printf("*** to point to the correct copy of ginac-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_ginac=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CPPFLAGS="$ac_save_CPPFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_ginac" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$GINACLIB_CONFIG" = "no" ; then
       echo "*** The ginac-config script installed by GiNaC could not be found"
       echo "*** If GiNaC was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the GINACLIB_CONFIG environment variable to the"
       echo "*** full path to ginac-config."
     else
       if test -f conf.ginactest ; then
        :
       else
          echo "*** Could not run GiNaC test program, checking why..."
          CPPFLAGS="$CFLAGS $GINACLIB_CPPFLAGS"
          LIBS="$LIBS $GINACLIB_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
#include <ginac/ginac.h>
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding GiNaC or finding the wrong"
          echo "*** version of GiNaC. If it is not finding GiNaC, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means GiNaC was incorrectly installed"
          echo "*** or that you have moved GiNaC since it was installed. In the latter case, you"
          echo "*** may want to edit the ginac-config script: $GINACLIB_CONFIG" ])
          CPPFLAGS="$ac_save_CPPFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     GINACLIB_CPPFLAGS=""
     GINACLIB_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(GINACLIB_CPPFLAGS)
  AC_SUBST(GINACLIB_LIBS)
  rm -f conf.ginactest
])
