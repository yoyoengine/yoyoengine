#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>

#include <stdlib.h>

void runscript(char*name) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    // Load and run a Lua script
    if (luaL_loadfile(L, name) || lua_pcall(L, 0, 0, 0)) {
        fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
        lua_close(L);
        exit(1);
    }
    else{
        lua_close(L);
    }
}