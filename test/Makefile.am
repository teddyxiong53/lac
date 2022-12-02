
bin_PROGRAMS = test-tinyev  test-tinyparam


AM_CFLAGS = \
	-I$(top_srcdir)/cjson \
    -I$(top_srcdir)/tinyev \
    -I$(top_srcdir)/tinyparam \
    -I$(top_srcdir)/tinyipc

LDADD = -L$(top_srcdir)/cjson -lcjson -lpthread -lm