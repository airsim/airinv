## server sub-directory
include $(top_srcdir)/Makefile.common
include $(srcdir)/sources.mk

# Binaries (server)
bin_PROGRAMS = airinvServer

airinvServer_SOURCES= $(svr_h_sources) $(svr_cc_sources) 
airinvServer_CXXFLAGS = $(BOOST_CFLAGS)
airinvServer_LDADD =
airinvServer_LDFLAGS = $(BOOST_LIBS) \
	$(BOOST_DATE_TIME_LIB) $(BOOST_ASIO_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) \
	$(top_builddir)/stdair/core/libstdair.la \
	$(top_builddir)/airinv/core/libairinv.la
