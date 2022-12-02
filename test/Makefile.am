
bin_PROGRAMS = test-tinyev  test-tinyparam


AM_CFLAGS = \
	-I$(top_srcdir)/cjson \
    -I$(top_srcdir)/tinyev \
    -I$(top_srcdir)/tinyparam \
    -I$(top_srcdir)/tinyipc \
	-I$(top_srcdir)/include

LDADD = -L$(top_srcdir)/cjson -L$(top_srcdir)/tinyparam -ltinyparam -lcjson -lpthread -lm