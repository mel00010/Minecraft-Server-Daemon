AC_INIT([minecraft-server-service], [1.0], [bug-automake@gnu.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CXX
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_LANG_PUSH([C++])

#~ PKG_PROG_PKG_CONFIG
#~ PKG_CHECK_MODULES([pstreams],[libpstreams-dev])
PKG_CHECK_MODULES([log4cpp],[log4cpp])
AC_CHECK_HEADERS([pstreams/pstream.h],
[],
[AC_MSG_ERROR([Could not find or include pstreams/pstream.h.  Is libpstreams-dev installed?])],
[])

AC_OUTPUT
