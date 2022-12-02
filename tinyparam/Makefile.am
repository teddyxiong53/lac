lib_LTLIBRARIES = libtinyparam.la

AM_CFLAGS = \
	-I$(top_srcdir)/include \
    -I$(top_srcdir)/tinyparam \
    -I$(top_srcdir)/cjson


libtinyparam_la_SOURCES = \
	tinyparam.c

LDADD = -L$(top_srcdir)/cjson -lcjson
