
target("lua")
    set_kind("shared")
    add_files("lua5.3/lapi.c          \
lua5.3/lauxlib.c       \
lua5.3/lbaselib.c      \
lua5.3/lbitlib.c       \
lua5.3/lcode.c         \
lua5.3/lcorolib.c      \
lua5.3/lctype.c        \
lua5.3/ldblib.c        \
lua5.3/ldebug.c        \
lua5.3/ldo.c           \
lua5.3/ldump.c         \
lua5.3/lfunc.c         \
lua5.3/lgc.c           \
lua5.3/linit.c         \
lua5.3/liolib.c        \
lua5.3/llex.c          \
lua5.3/lmathlib.c      \
lua5.3/lmem.c          \
lua5.3/loadlib.c       \
lua5.3/lobject.c       \
lua5.3/lopcodes.c      \
lua5.3/loslib.c        \
lua5.3/lparser.c       \
lua5.3/lstate.c        \
lua5.3/lstring.c       \
lua5.3/lstrlib.c       \
lua5.3/ltable.c        \
lua5.3/ltablib.c       \
lua5.3/ltests.c        \
lua5.3/ltm.c           \
lua5.3/lua.c           \
lua5.3/lundump.c       \
lua5.3/lutf8lib.c      \
lua5.3/lvm.c           \
lua5.3/lzio.c")