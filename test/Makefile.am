
bin_PROGRAMS = test-libev  test-tinyparam test-jsonrpc-server \
	test-jsonrpc-client unix-echo-client unix-echo-server \
	test-thpool test-libev-async \
	test-libev-3-basic-event test-libev-child test-libev-stat \
	test-libev-periodic test-libev-cleanup test-libev-prepare-check \
	test-event-handler \
	test-dl-list \
	test-libevent-hello \
	test-lua \
	test-lua-as-config \
	test-elk \
	test-elk-cmd \
	test-elk-rpc


AM_CFLAGS = \
	-I$(top_srcdir)/cjson \
    -I$(top_srcdir)/libev \
	-I$(top_srcdir)/libevent/include \
    -I$(top_srcdir)/tinyparam \
    -I$(top_srcdir)/tinyipc \
	-I$(top_srcdir)/jsonrpc \
	-I$(top_srcdir)/tinythpool \
	-I$(top_srcdir)/event_handler \
	-I$(top_srcdir)/tinyds \
	-I$(top_srcdir)/lua/lua5.3 \
	-I$(top_srcdir)/pikapython/pikascript-api \
    -I$(top_srcdir)/pikapython/pikascript-core \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/elk

AM_CFLAGS += -g -O0 -rdynamic -funwind-tables

LDADD = -L$(top_srcdir)/cjson -L$(top_srcdir)/tinyparam \
	-L$(top_srcdir)/libev \
	-L$(top_srcdir)/libevent \
	-L$(top_srcdir)/jsonrpc \
	-L$(top_srcdir)/tinythpool \
	-L$(top_srcdir)/event_handler \
	-L$(top_srcdir)/lua \
	-L$(top_srcdir)/elk \
	-L$(top_srcdir)/pikapython \
	-ljsonrpc \
	-ltinyparam \
	-ltinythpool \
	-leventhandler \
	-lev -levent -llua  -lelk -lcjson -lpthread -lm