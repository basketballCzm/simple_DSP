#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([simle_DSP], [0.0.1], [caster_zeng@wushuu.com])
AC_CONFIG_SRCDIR([adstat.cpp])
AC_CONFIG_HEADERS([config.h])


AC_DEFINE([AD_DATA_FILE], ["/tmp/advertisement.data"], [ad data file path])
AC_DEFINE([USER_MAP_FILE],["/tmp/user_map.data"],[users map file path])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PREFIX_DEFAULT([/usr/local/nginx/simple_DSP_cgi])
# Checks for programs.
AC_PROG_CXX([clang++ g++])
AC_PROG_CC([clang gcc])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([fcgi_config.h],[],[AC_MSG_ERROR([Unable to find fcgi, please install fcgi])])
AC_CHECK_HEADERS([uriparser/Uri.h],[],[AC_MSG_ERROR([Unable to find uriparser, please install uriparser])])
# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
