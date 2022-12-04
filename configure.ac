# linux-app-component(lac) - configure.ac
# Copyright (c) 2022 teddyxiong53

AC_PREREQ([2.59])
AC_INIT([lac], [1.0.0], [1073167306@qq.com])
AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_AR
AM_PROG_CC_C_O
LT_INIT

AC_PREFIX_DEFAULT([/usr])
AC_SUBST([runstatedir], [/var/run])

# support for debugging
AC_ARG_ENABLE([debug],
	AS_HELP_STRING([--enable-debug], [enable debugging support]))
AM_CONDITIONAL([ENABLE_DEBUG], [test "x$enable_debug" = "xyes"])
AM_COND_IF([ENABLE_DEBUG], [
	AC_DEFINE([DEBUG], [1], [Define to 1 if the debugging is enabled.])
])


AC_CHECK_LIB([pthread], [pthread_create],
	[], [AC_MSG_ERROR([pthread library not found])])

AC_CONFIG_FILES([
	Makefile
	cjson/Makefile
	tinyipc/Makefile
    libev/Makefile
	tinyparam/Makefile
	jsonrpc/Makefile
	tinyutils/Makefile
	tinythpool/Makefile
	event_handler/Makefile
    demo/Makefile
	test/Makefile
	])
AC_OUTPUT
