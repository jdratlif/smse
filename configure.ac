dnl Initialize autoconf
AC_INIT([smse], [0.91], [webmaster@technoplaza.net])

dnl require autoconf >= 2.59
AC_PREREQ([2.59])

dnl use the config.h header
AC_CONFIG_HEADERS([config.h])

dnl initialize automake and allow bzipped distributions
AM_INIT_AUTOMAKE([1.9.6 dist-bzip2 dist-zip])

dnl host system check
AC_CANONICAL_HOST

dnl AM_CONDITIONAL([WINDOWS], test x$host_os = xmingw32)
AM_CONDITIONAL([WINDOWS],
               [case $host_os in
               mingw32 | cygwin ) test x = x ;;
               * ) test 0 = 1 ;;
               esac])

dnl check for c++ compiler and BSD install
AC_PROG_CXX
AC_PROG_INSTALL

dnl tests for wxWidgets
AM_OPTIONS_WXCONFIG
AM_PATH_WXCONFIG([2.6.0], HAVE_WXWIDGETS=1, HAVE_WXWIDGETS=0)

if test $HAVE_WXWIDGETS -eq 1; then
    CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
    CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
    LIBS="$LIBS $WX_LIBS"
else
    AC_MSG_ERROR([wxWidgets >= 2.6.1 is required.
                  
                  Please install wxWidgets 2.6.1 from http://www.wxwidgets.org.
                  
                  This error could also be the result of a non-standard install
                  path or a missing wx-config program.])
fi

WX_INCLUDES=""
for opt in $WX_CPPFLAGS
do
    case "$opt" in
        -I*)
            WX_INCLUDES="$WX_INCLUDES $opt"
            ;;
    esac
done
AC_SUBST(WX_INCLUDES)

dnl configure files to produce
AC_CONFIG_FILES([Makefile source/Makefile])
AC_OUTPUT

