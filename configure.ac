#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([Ji3], [3.0.1], [], [ji3], [http://fhoerni.free.fr/ji3/])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB

# Set a OS specific settings
WINDOWS=no
MINGW=no
CYGWIN=no
case $target_os in
*linux*)
  ;;
*darwin*)
;;
GNU/kFreeBSD)
;;
*mingw*)
WINDOWS=yes
MINGW=yes
;;
*cygwin*)
WINDOWS=yes
CYGWIN=yes
  ;;
GNU)
  ;;
esac
AM_CONDITIONAL(WINDOWS, test x$WINDOWS = xyes)
AM_CONDITIONAL(CYGWIN, test x$MINGW = xyes)
AM_CONDITIONAL(MINGW, test x$MINGW = xyes)

# Checks for external libraries.
PKG_PROG_PKG_CONFIG()
PKG_CHECK_MODULES(SDL, sdl)
AC_CHECK_LIB([SDL_mixer], [Mix_OpenAudio])


if test x$WINDOWS = xyes; then
    AC_ARG_VAR([WINE],     [Wine translator, used to run Inno Setup])
    AC_CHECK_PROG(WINE, wine, wine)
    if test -z "$WINE" ; then
        AC_MSG_ERROR([Please install wine before cross-compiling.])
    fi
    
    ISCC="$(find ~/.wine/drive_c -type f -name ISCC.exe)"
    if test -z "$ISCC" ; then
        AC_MSG_ERROR([Please install Inno Setup into wine before cross-compiling.])
    fi
    AC_SUBST(ISCC)
    AC_CHECK_LIB([QtCore4], [QString])
    AC_CHECK_LIB([QtGui4], [QApplication])

else

    PKG_CHECK_MODULES(QTCORE, QtCore)
    PKG_CHECK_MODULES(QTGUI, QtGui)
fi

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h strings.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday localtime_r])

AC_CONFIG_FILES([Makefile
                 test/Makefile
                 ji3.iss])

AC_OUTPUT

