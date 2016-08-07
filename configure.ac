AC_INIT([minecraft-server-daemon], [1.0], [bug-automake@gnu.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_PROG_CXX
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES(
	[
 		Makefile
 		src/daemon/Makefile
 		src/client/Makefile
 		docs/Doxyfile
 		docs/Makefile
	]
)
AC_LANG_PUSH([C++])

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not bfound - continuing without Doxygen support])
fi
AM_CONDITIONAL([HAVE_DOXYGEN], 
[test -n "$DOXYGEN"])#AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/Doxyfile])])
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


# Check for dependencies
PKG_CHECK_MODULES([libevent],[libevent])
PKG_CHECK_MODULES([log4cpp],[log4cpp])
PKG_CHECK_MODULES([ncurses],[ncurses])

# Create --enable-debug option, allowing debugging to be switched on and off 
AC_ARG_ENABLE([debug],
	[  --enable-debug    Turn on debugging],
	[case "${enableval}" in
		yes)   debug=true  ;;
		no)    debug=false ;;
		true)  debug=true  ;;
		false) debug=false ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
	esac],
	[debug=false]
)
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

AC_OUTPUT

