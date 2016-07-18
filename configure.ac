AC_INIT([minecraft-server-daemon], [1.0], [bug-automake@gnu.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_PROG_CXX
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/daemon/Makefile
 src/client/Makefile
])
AC_LANG_PUSH([C++])

#~ PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([libevent],[libevent])
PKG_CHECK_MODULES([log4cpp],[log4cpp])
PKG_CHECK_MODULES([ncurses],[ncurses])
AC_ARG_ENABLE([debug],
[  --enable-debug    Turn on debugging],
[case "${enableval}" in
  yes) debug=true ;;
  no)  debug=true ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],[debug=false])
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])
AC_OUTPUT
