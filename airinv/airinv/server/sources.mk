svr_h_sources = \
	$(top_srcdir)/airinv/server/header.hpp \
	$(top_srcdir)/airinv/server/connection.hpp \
	$(top_srcdir)/airinv/server/mime_types.hpp \
	$(top_srcdir)/airinv/server/reply.hpp \
	$(top_srcdir)/airinv/server/request.hpp \
	$(top_srcdir)/airinv/server/request_handler.hpp \
	$(top_srcdir)/airinv/server/request_parser.hpp \
	$(top_srcdir)/airinv/server/server.hpp
svr_cc_sources = \
	$(top_srcdir)/airinv/server/connection.cpp \
	$(top_srcdir)/airinv/server/mime_types.cpp \
	$(top_srcdir)/airinv/server/reply.cpp \
	$(top_srcdir)/airinv/server/request_handler.cpp \
	$(top_srcdir)/airinv/server/request_parser.cpp \
	$(top_srcdir)/airinv/server/win_main.cpp \
	$(top_srcdir)/airinv/server/posix_main.cpp \
	$(top_srcdir)/airinv/server/server.cpp
