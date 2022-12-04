lib_LTLIBRARIES = libcjson.la

AM_CFLAGS = \
	-I$(top_srcdir)/include
AM_CFLAGS += -g -O0 -rdynamic -funwind-tables
libcjson_la_SOURCES = \
	cJSON.c

LDADD = -lm
