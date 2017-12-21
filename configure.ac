#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([mathformula], [1.0.0], [pyqhaoku@163.com])
AC_CONFIG_SRCDIR([src/pstring.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([mathformula], [1.0.0])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_PROG_RANLIB

# FIXME: Replace `main' with a function in `-lm':
#AC_CHECK_LIB([m], [main])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strdup strstr])

AC_CONFIG_FILES([Makefile
				 src/Makefile])
AC_OUTPUT
