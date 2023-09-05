#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>

#include <stdlib.h>

lua_State *L;

void lua_engine_test(){
    printf("Hello from lua_engine_test\n");
}

void runscript(char*name) {
    // Load and run a Lua script
    if (luaL_loadfile(L, name) || lua_pcall(L, 0, 0, 0)) {
        fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
        exit(1);
    }
}

void lua_yoyoengine_dispatch(lua_State* L){
    L = luaL_newstate();
    luaL_openlibs(L);
}

void lua_init(){
    L = luaL_newstate();
    luaL_openlibs(L);
}

void lua_shutdown(){
    lua_close(L);
}