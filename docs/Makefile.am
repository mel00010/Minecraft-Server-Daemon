if HAVE_DOXYGEN
if DOXYGEN_CREATE_MAN
dist_man_MANS = man/man3/man_page_1.3 man/man3/man_page_2.3
man/man3/man_page_1.3: doxyfile.stamp
man/man3/man_page_2.3: doxyfile.stamp
endif

doxyfile.stamp:
	$(DOXYGEN) Doxyfile
	echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp
clean-local:
if DOXYGEN_CREATE_HTML
	rm -rf $(top_srcdir)/docs/html
endif
if DOXYGEN_CREATE_LATEX
	rm -rf $(top_srcdir)/docs/latex
endif
if DOXYGEN_CREATE_MAN
	rm -rf $(top_srcdir)/docs/man
endif
if DOXYGEN_CREATE_RTF
	rm -rf $(top_srcdir)/docs/rtf
endif
if DOXYGEN_CREATE_XML
	rm -rf $(top_srcdir)/docs/xml
endif
if DOXYGEN_CREATE_DOCBOOK
	rm -rf $(top_srcdir)/docs/docbook
endif
endif