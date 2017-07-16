dnl $Id$
dnl config.m4 for extension ioc

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(ioc, for ioc support,
dnl Make sure that the comment is aligned:
dnl [  --with-ioc             Include ioc support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(ioc, whether to enable ioc support,
dnl Make sure that the comment is aligned:
[  --enable-ioc           Enable ioc support])

if test "$PHP_IOC" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-ioc -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/ioc.h"  # you most likely want to change this
  dnl if test -r $PHP_IOC/$SEARCH_FOR; then # path given as parameter
  dnl   IOC_DIR=$PHP_IOC
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for ioc files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       IOC_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$IOC_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the ioc distribution])
  dnl fi

  dnl # --with-ioc -> add include path
  dnl PHP_ADD_INCLUDE($IOC_DIR/include)

  dnl # --with-ioc -> check for lib and symbol presence
  dnl LIBNAME=ioc # you may want to change this
  dnl LIBSYMBOL=ioc # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $IOC_DIR/$PHP_LIBDIR, IOC_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_IOCLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong ioc lib version or lib not found])
  dnl ],[
  dnl   -L$IOC_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(IOC_SHARED_LIBADD)

  PHP_NEW_EXTENSION(ioc, ioc.c, $ext_shared)
fi
