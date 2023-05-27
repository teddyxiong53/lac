lib_LTLIBRARIES = libmongoose.la

AM_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)


AM_CFLAGS += -g -O0 -rdynamic -funwind-tables


libmongoose_la_SOURCES = \
base64.c    \
dns.c       \
event.c     \
fmt.c       \
fs.c        \
fs_posix.c  \
http.c      \
iobuf.c     \
json.c      \
log.c       \
md5.c       \
mqtt.c      \
net.c       \
tls_dummy.c       \
printf.c    \
queue.c     \
rpc.c       \
sha1.c      \
sock.c      \
str.c       \
timer.c     \
url.c       \
ssi.c       \
util.c      \
ws.c


LDADD = -lm
