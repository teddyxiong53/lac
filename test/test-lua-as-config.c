/*
    这个是把lua作为配置文件来使用。
    https://blog.csdn.net/losophy/article/details/9003473
*/
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "mylog.h"


void read_var(lua_State *L, int *w, int *h)
{
    lua_getglobal(L, "width");//获取lua中的变量的之，并放入到栈中。
    lua_getglobal(L, "height");
    if (!lua_isnumber(L, -2)) {
        // -1表示栈顶，-2表示栈顶的下面一个元素。
        luaL_error(L, "width shoud be a number");
    }
    if (!lua_isnumber(L, -1)) {
        luaL_error(L, "height should be a number");
    }
    *w = (int)lua_tonumber(L, -2);
    *h = (int)lua_tonumber(L, -1);
    mylogd("w:%d, h:%d", *w, *h);
}

void read_table_with_item(lua_State *L, char *tname, char *item)
{
    lua_getglobal(L, tname);
    lua_pushstring(L, item);//首先把元素的名字压入栈顶。
    lua_gettable(L, -2);
    mylogd("%s.%s=%d", tname, item, (int)lua_tonumber(L, -1));
    lua_pop(L, 2);
}

int main(int argc, char const *argv[])
{
    char *file = "./config-test.lua";
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    if (luaL_loadfile(L, file) || lua_pcall(L, 0,0,0)) {
        // 说明出错了
        myloge("lua load file fail");

    }
    int w =0, h=0;
    read_var(L, &w, &h);
    read_table_with_item(L, "t1", "a");
    lua_close(L);
    return 0;
}
