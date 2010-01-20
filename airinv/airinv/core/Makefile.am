# core
include $(top_srcdir)/Makefile.common
include $(srcdir)/sources.mk

## Source directory

MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =


# Library
lib_LTLIBRARIES = libairinv.la

libairinv_la_SOURCES = $(service_h_sources) $(service_cc_sources)
libairinv_la_LIBADD = \
	$(top_builddir)/airinv/basic/libbas.la \
	$(top_builddir)/airinv/bom/libbom.la \
	$(top_builddir)/airinv/factory/libfac.la \
	$(top_builddir)/airinv/command/libcmd.la \
	$(top_builddir)/airinv/service/libsvc.la
libairinv_la_LDFLAGS = \
	$(BOOST_DATE_TIME_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) \
	$(top_builddir)/stdair/core/libstdair.la \
	-version-info $(GENERIC_LIBRARY_VERSION)
