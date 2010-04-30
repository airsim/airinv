## server sub-directory
include $(top_srcdir)/Makefile.common
include $(srcdir)/sources.mk

# Binaries (server)
bin_PROGRAMS = airinvServer airinvClient

airinvServer_SOURCES= $(svr_h_sources) $(svr_cc_sources) 
airinvServer_CXXFLAGS = $(BOOST_CFLAGS)
airinvServer_LDADD =
airinvServer_LDFLAGS = $(BOOST_LIBS) \
	$(BOOST_DATE_TIME_LIB) $(BOOST_ASIO_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) \
	$(top_builddir)/stdair/core/libstdair.la \
	$(top_builddir)/airinv/core/libairinv.la

airinvClient_SOURCES= $(clt_h_sources) $(clt_cc_sources) 
airinvClient_CXXFLAGS = $(BOOST_CFLAGS)
airinvClient_LDADD =
airinvClient_LDFLAGS = $(BOOST_LIBS) \
	$(BOOST_DATE_TIME_LIB) $(BOOST_ASIO_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) \
	$(top_builddir)/stdair/core/libstdair.la
