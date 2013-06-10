# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([mpa_robot],[1.0],[emmanuel.leblond@gmail.com])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_LIBTOOL
AC_LANG(C++,C)

# Checks for libraries.


# Checks for header files.
AC_CHECK_LIB([sfml-system], [main], ,
AC_MSG_ERROR([ *** Unable to find SFML library (http://www.sfml-dev.org/)]))
AC_CHECK_HEADERS([SFML/System.hpp], ,
AC_MSG_ERROR([ *** SFML library found but cannot find headers (http://www.sfml-dev.org/)]))

AC_CHECK_LIB([sfml-window], [main], ,
AC_MSG_ERROR([ *** Unable to find SFML Window library (http://www.sfml-dev.org/)]))
AC_CHECK_HEADERS([SFML/Window.hpp], ,
AC_MSG_ERROR([ *** SFML Window library found but cannot find headers (http://www.sfml-dev.org/)]))

AC_CHECK_LIB([sfml-graphics], [main], ,
AC_MSG_ERROR([ *** Unable to find SFML Graphics library (http://www.sfml-dev.org/)]))
AC_CHECK_HEADERS([SFML/Graphics.hpp], ,
AC_MSG_ERROR([ *** SFML Graphics library found but cannot find headers (http://www.sfml-dev.org/)]))

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_ENABLE_SHARED(yes)
LT_INIT([disable-static])

AC_CONFIG_FILES([Makefile
   src/Makefile
   src/robot/Makefile])

AC_OUTPUT
