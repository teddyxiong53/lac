lib_LTLIBRARIES = libjsonrpc.la

libjsonrpc_la_SOURCES = jsonrpc_server.c jsonrpc_client.c backtrace_print.c

AM_CFLAGS = \
	-I$(top_srcdir)/include \
    -I$(top_srcdir)/libev \
    -I$(top_srcdir)/cjson
AM_CFLAGS += -g -O0 -rdynamic -funwind-tables