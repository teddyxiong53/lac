
bin_PROGRAMS = test-libev  test-tinyparam test-jsonrpc


AM_CFLAGS = \
	-I$(top_srcdir)/cjson \
    -I$(top_srcdir)/libev \
    -I$(top_srcdir)/tinyparam \
    -I$(top_srcdir)/tinyipc \
	-I$(top_srcdir)/jsonrpc \
	-I$(top_srcdir)/include

LDADD = -L$(top_srcdir)/cjson -L$(top_srcdir)/tinyparam \
	-L$(top_srcdir)/libev \
	-L$(top_srcdir)/jsonrpc \
	-ljsonrpc \
	-ltinyparam \
	-lev -lcjson -lpthread -lm