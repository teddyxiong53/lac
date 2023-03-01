lib_LTLIBRARIES = libpikapython.la


libpikapython_la_SOURCES = \
pikascript-api/__asset_pikaModules_py_a.c  \
pikascript-api/__pikaBinding.c  \
pikascript-api/pikaScript.c \
pikascript-lib/PikaStdLib/PikaDebuger_Debuger.c \
pikascript-lib/PikaStdLib/PikaStdData_ByteArray.c \
pikascript-lib/PikaStdLib/PikaStdData_Dict.c \
pikascript-lib/PikaStdLib/PikaStdData_FILEIO.c \
pikascript-lib/PikaStdLib/PikaStdData_List.c \
pikascript-lib/PikaStdLib/PikaStdData_String.c \
pikascript-lib/PikaStdLib/PikaStdData_Tuple.c \
pikascript-lib/PikaStdLib/PikaStdData_Utils.c \
pikascript-lib/PikaStdLib/PikaStdLib_MemChecker.c \
pikascript-lib/PikaStdLib/PikaStdLib_RangeObj.c \
pikascript-lib/PikaStdLib/PikaStdLib_StringObj.c \
pikascript-lib/PikaStdLib/PikaStdLib_SysObj.c \
pikascript-lib/PikaStdLib/PikaStdTask_Task.c \
pikascript-core/BaseObj.c            \
pikascript-core/dataArg.c            \
pikascript-core/dataArgs.c           \
pikascript-core/dataLink.c           \
pikascript-core/dataLinkNode.c       \
pikascript-core/dataMemory.c         \
pikascript-core/dataQueue.c          \
pikascript-core/dataQueueObj.c       \
pikascript-core/dataStack.c          \
pikascript-core/dataString.c         \
pikascript-core/dataStrs.c           \
pikascript-core/PikaCompiler.c       \
pikascript-core/PikaObj.c            \
pikascript-core/PikaParser.c         \
pikascript-core/PikaPlatform.c       \
pikascript-core/PikaVM.c             \
pikascript-core/TinyObj.c


AM_CFLAGS = \
	-I$(top_srcdir)/include \
    -I$(top_srcdir)/pikapython/pikascript-api \
    -I$(top_srcdir)/pikapython/pikascript-core

AM_CFLAGS += -g -O0 -rdynamic -funwind-tables

LDADD = -L$(top_srcdir)/cjson -L$(top_srcdir)/tinyparam \
	-L$(top_srcdir)/libev \
	-L$(top_srcdir)/jsonrpc \
	-L$(top_srcdir)/tinythpool \
	-L$(top_srcdir)/event_handler \
    -L$(top_srcdir)/lua \
	-ljsonrpc \
	-ltinyparam \
	-ltinythpool \
	-leventhandler \
	-lev -lcjson -lpthread -lm