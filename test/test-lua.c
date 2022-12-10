#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"


int main(void)
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaL_dofile(L, "./helloworld.lua");
    lua_close(L);
    return 0;
}