dnl Initialize autoconf
AC_INIT([smse], [1.0b], [webmaster@technoplaza.net])

dnl require autoconf >= 2.69
AC_PREREQ([2.69])

dnl use the config.h header
AC_CONFIG_HEADERS([config.h])

dnl initialize automake and allow bzipped distributions
AM_INIT_AUTOMAKE([1.16.1 dist-bzip2])

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
AM_PATH_WXCONFIG([2.8.0], HAVE_WXWIDGETS=1, HAVE_WXWIDGETS=0)

if test $HAVE_WXWIDGETS -eq 1; then
    CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
    CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
    LIBS="$LIBS $WX_LIBS"
else
    AC_MSG_ERROR([wxWidgets 2.8 is required.
                  Please install wxWidgets from http://www.wxwidgets.org.
                  
                  This error could also be the result of a non-standard install
                  path or a missing wx-config program.])
fi

dnl add compile options for older versions of OS X
AC_ARG_WITH([macosx-version-min],
    AS_HELP_STRING(
        [--with-macosx-version-min=VERSION],
        [compile for this version of Mac OS X and later]))

if test "x$with_macosx_version_min" != "x"; then
    MACOSX_VERSION_MIN_OPTS="-mmacosx-version-min=$with_macosx_version_min"
    CPPFLAGS="$CPPFLAGS $MACOSX_VERSION_MIN_OPTS"
    CFLAGS="$CFLAGS $MACOSX_VERSION_MIN_OPTS"
    CXXFLAGS="$CXXFLAGS $MACOSX_VERSION_MIN_OPTS"
    LDFLAGS="$LDFLAGS $MACOSX_VERSION_MIN_OPTS"
fi

dnl save CPP include paths to use with windows resource compiler in msys/cygwin
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
