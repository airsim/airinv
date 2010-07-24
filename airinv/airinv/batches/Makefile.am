# airinv/batches
include $(top_srcdir)/Makefile.common

## Source directory

MAINTAINERCLEANFILES = Makefile.in


# Binaries (batches)
bin_PROGRAMS = airinv readInventory

#
airinv_SOURCES = $(airinv_batches_h_sources) $(airinv_batches_cc_sources)
airinv_CXXFLAGS = $(BOOST_CFLAGS)
airinv_LDADD =
airinv_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LIB) \
	$(top_builddir)/stdair/core/libstdair.la \
	$(top_builddir)/airinv/core/libairinv.la

readInventory_SOURCES = readInventory.cpp
readInventory_CXXFLAGS = $(BOOST_CFLAGS)
readInventory_LDADD =
readInventory_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LIB) \
	$(top_builddir)/stdair/core/libstdair.la \
	$(top_builddir)/airinv/core/libairinv.la
