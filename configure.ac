AC_INIT([minecraft-server-daemon], [1.0], [bug-automake@gnu.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_PROG_CXX
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES(
	[
 		Makefile
 		daemon/src/Makefile
 		client/src/Makefile
 		docs/Doxyfile
 		docs/Makefile
 		config/config.json
 		config/Makefile
	]
)
AC_LANG_PUSH([C++])

# Check for dependencies
PKG_CHECK_MODULES([libevent],[libevent])
PKG_CHECK_MODULES([log4cpp],[log4cpp])
PKG_CHECK_MODULES([ncurses],[ncurses])

# Test for doxyen's existence.  If true, define the HAVE_DOXYGEN variable. 
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi
AM_CONDITIONAL([HAVE_DOXYGEN], 
[test -n "$DOXYGEN"])


# Add command line options for configuring doxygen output.
AC_ARG_ENABLE([doxygen_create_html],
	[  --enable-doxygen_create_html       Enable creation of HTML documentation by doxygen],
	[case "${enableval}" in
		yes)   doxygen_create_html=true  ;;
		no)    doxygen_create_html=false ;;
		true)  doxygen_create_html=true  ;;
		false) doxygen_create_html=false ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-doxygen_create_html]) ;;
	esac],
	[doxygen_create_html=true]
)
AM_CONDITIONAL([DOXYGEN_CREATE_HTML], [test x$doxygen_create_html = xtrue])


AC_ARG_ENABLE([doxygen_create_latex],
	[  --enable-doxygen_create_latex      Enable creation of LaTeX documentation by doxygen],
	[case "${enableval}" in
		yes)   doxygen_create_latex=true  ;;
		no)    doxygen_create_latex=false ;;
		true)  doxygen_create_latex=true  ;;
		false) doxygen_create_latex=false ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-doxygen_create_latex]) ;;
	esac],
	[doxygen_create_latex=false]
)
AM_CONDITIONAL([DOXYGEN_CREATE_LATEX], [test x$doxygen_create_latex = xtrue])

AC_ARG_ENABLE([doxygen_create_man],
	[  --enable-doxygen_create_man        Enable creation of Man documentation by doxygen],
	[case "${enableval}" in
		yes)   doxygen_create_man=true  ;;
		no)    doxygen_create_man=false ;;
		true)  doxygen_create_man=true  ;;
		false) doxygen_create_man=false ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-doxygen_create_man]) ;;
	esac],
	[doxygen_create_man=true]
)
AM_CONDITIONAL([DOXYGEN_CREATE_MAN], [test x$doxygen_create_man = xtrue])


AC_ARG_ENABLE([doxygen_create_rtf],
	[  --enable-doxygen_create_rtf        Enable creation of RTF documentation by doxygen],
	[case "${enableval}" in
		yes)   doxygen_create_rtf=true  ;;
		no)    doxygen_create_rtf=false ;;
		true)  doxygen_create_rtf=true  ;;
		false) doxygen_create_rtf=false ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-doxygen_create_rtf]) ;;
	esac],
	[doxygen_create_rtf=false]
)
AM_CONDITIONAL([DOXYGEN_CREATE_RTF], [test x$doxygen_create_rtf = xtrue])

AC_ARG_ENABLE([doxygen_create_xml],
	[  --enable-doxygen_create_xml        Enable creation of XML documentation by doxygen],
	[case "${enableval}" in
		yes)   doxygen_create_xml=true  ;;
		no)    doxygen_create_xml=false ;;
		true)  doxygen_create_xml=true  ;;
		false) doxygen_create_xml=false ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-doxygen_create_xml]) ;;
	esac],
	[doxygen_create_xml=false]
)
AM_CONDITIONAL([DOXYGEN_CREATE_XML], [test x$doxygen_create_xml = xtrue])

AC_ARG_ENABLE([doxygen_create_docbook],
	[  --enable-doxygen_create_docbook    Enable creation of Docbook documentation by doxygen],
	[case "${enableval}" in
		yes)   doxygen_create_docbook=true  ;;
		no)    doxygen_create_docbook=false ;;
		true)  doxygen_create_docbook=true  ;;
		false) doxygen_create_docbook=false ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-doxygen_create_docbook]) ;;
	esac],
	[doxygen_create_docbook=false]
)
AM_CONDITIONAL([DOXYGEN_CREATE_DOCBOOK], [test x$doxygen_create_docbook = xtrue])


#Add output variables allowing for configuration of hard coded program options
AC_ARG_VAR([LOG_DIR], [Directory logs will be put in.])
if test -z "$LOG_DIR";
	then LOG_DIR="/var/log/minecraft" ; AC_MSG_NOTICE(LOG_DIR = $LOG_DIR)
else
	AC_MSG_NOTICE(LOG_DIR = $LOG_DIR)
fi

AC_DEFINE_UNQUOTED([__LOG_DIR__], ["$LOG_DIR"], [Log directory path])

AC_ARG_VAR([CONFIG_DIR_NAME], [Name of configuration directory in sysconfdir.])
if test -z "$CONFIG_DIR_NAME";
	then CONFIG_DIR_NAME="minecraft" ; AC_MSG_NOTICE(CONFIG_DIR_NAME = $CONFIG_DIR_NAME)
else
	AC_MSG_NOTICE(CONFIG_DIR_NAME = $CONFIG_DIR_NAME)
fi

AC_ARG_VAR([CONFIG_FILE_NAME], [Absolute path of the daemon configuration file.])
if test -z "$CONFIG_FILE_NAME";
	then CONFIG_FILE_NAME="/usr/local/etc/$CONFIG_DIR_NAME/config.json" ; AC_MSG_NOTICE(CONFIG_FILE_NAME = $CONFIG_FILE_NAME)
else
	AC_MSG_NOTICE(CONFIG_FILE_NAME = $CONFIG_FILE_NAME)
fi
AC_DEFINE_UNQUOTED([__CONFIG_FILE_NAME__], ["$CONFIG_FILE_NAME"], [Config file path])

AC_ARG_VAR([DEFAULT_SERVER_DIR], [Default directory where servers are assumed to be located if an absolute path is not given in the config file.])
if test -z "$DEFAULT_SERVER_DIR";
	then DEFAULT_SERVER_DIR="/usr/local/var/$CONFIG_DIR_NAME/servers/" ; AC_MSG_NOTICE(DEFAULT_SERVER_DIR = $DEFAULT_SERVER_DIR)
else
	AC_MSG_NOTICE(DEFAULT_SERVER_DIR = $DEFAULT_SERVER_DIR)
fi

# Create --enable-debug option, allowing debugging to be switched on and off 
AC_ARG_ENABLE([debug],
	[  --enable-debug    Turn on debugging],
	[case "${enableval}" in
		yes)   debug=true; AC_DEFINE([__DEBUGGING__], [], [Debugging]  ;;
		no)    debug=false;                           	               ;;
		true)  debug=true; AC_DEFINE([__DEBUGGING__], [], [Debugging]  ;;
		false) debug=false;                                            ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-debug])   ;;
	esac],
	[debug=false]
)
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

AC_OUTPUT

