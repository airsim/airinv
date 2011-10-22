## test/airinv sub-directory
include $(top_srcdir)/Makefile.common
include $(srcdir)/sources.mk

##
SUBDIRS =

EXTRA_DIST =

##
nofail_PRG = InventoryTestSuite
fail_PRG =

#
MAINTAINERCLEANFILES = Makefile.in
CLEANFILES = ${nofail_PRG:%=%_utfresults.xml} ${nofail_PRG:%=%.log}

#
check_PROGRAMS = $(nofail_PRG) $(fail_PRG)
TESTS = $(check_PROGRAMS)
XFAIL_TESTS = $(fail_PRG)

#
InventoryTestSuite_SOURCES = $(airinv_test_bin_cc_sources)
InventoryTestSuite_CXXFLAGS = $(BOOST_CFLAGS) $(STDAIR_CFLAGS)
InventoryTestSuite_LDADD =
InventoryTestSuite_LDFLAGS = \
	$(BOOST_LIBS) $(BOOST_UNIT_TEST_FRAMEWORK_LIB) $(STDAIR_LIBS) \
	$(top_builddir)/airinv/core/libairinv.la

