dnl Processs this file with autoconf to produce a configure script.
AC_INIT(mvtxonline2019, [1.00])
AC_CONFIG_SRCDIR([configure.ac])

AM_INIT_AUTOMAKE

AC_PROG_CXX(cxx CC g++)

AM_INIT_AUTOMAKE

AC_CANONICAL_HOST

LT_INIT([disable-static])

AC_PROG_INSTALL

dnl   no point in suppressing warnings people should
dnl   at least see them, so here we go for g++: -Wall
if test $ac_cv_prog_gxx = yes; then
   #CXXFLAGS="$CXXFLAGS -Wall -Werror"
   CXXFLAGS="$CXXFLAGS -Wall"
fi

ROOTLIBS=`root-config --libs`
ROOTINC=`root-config --incdir`
AC_SUBST(ROOTLIBS)
AC_SUBST(ROOTINC)

dnl test for root 6
if test `root-config --version | awk '{print $1=6.?"1":"0"}'` = 1; then
   CINTDEFS=" -noIncludePaths  -inlineInputHeader "
   AC_SUBST(CINTDEFS)
   AC_DEFINE(HAVE_ROOT6)
fi
AM_CONDITIONAL([MAKEROOT6],[test `root-config --version | awk '{print $1>=6.?"1":"0"}'` = 1])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
