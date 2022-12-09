bin_PROGRAMS = bgservice uiapp cliapp

bgservice_DEPENDENCIES = $(lib_LTLIBRARIES)
uiapp_DEPENDENCIES = $(lib_LTLIBRARIES)
cliapp_DEPENDENCIES = $(lib_LTLIBRARIES)

bgservice_SOURCES = bgservice-src/bgservice.c bgservice-src/main.c
uiapp_SOURCES = uiapp-src/main.c
cliapp_SOURCES = cliapp-src/main.c

AM_CFLAGS = \
	-I$(top_srcdir)/cjson \
    -I$(top_srcdir)/libev \
    -I$(top_srcdir)/tinyparam \
    -I$(top_srcdir)/tinyipc \
	-I$(top_srcdir)/jsonrpc \
	-I$(top_srcdir)/tinythpool \
	-I$(top_srcdir)/event_handler \
	-I$(top_srcdir)/include

AM_CFLAGS += -g -O0 -rdynamic -funwind-tables

LDADD = -L$(top_srcdir)/cjson -L$(top_srcdir)/tinyparam \
	-L$(top_srcdir)/libev \
	-L$(top_srcdir)/jsonrpc \
	-L$(top_srcdir)/tinythpool \
	-L$(top_srcdir)/event_handler \
	-ljsonrpc \
	-ltinyparam \
	-ltinythpool \
	-leventhandler \
	-lev -lcjson -lpthread -lm