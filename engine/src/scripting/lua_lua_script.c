/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <lua.h>

#include <yoyoengine/ecs/lua_script.h>
#include <yoyoengine/lua_api.h>
#include <yoyoengine/logging.h>



int ye_lua_create_lua_script(lua_State *L) {
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not create lua_script component: entity is null\n");
        return 0;
    }

    const char * script = luaL_checkstring(L, 2);

    ye_add_lua_script_component(ent, script, NULL);

    return 0;
}



int ye_lua_lua_script_query(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not query lua_script: entity is null\n");
        return 0;
    }

    lua_pushboolean(L, ent->lua_script->active);
    lua_pushstring(L, ent->lua_script->script_handle);

    return 2;
}

int ye_lua_lua_script_modify(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);
    
    if(ent == NULL) {
        ye_logf(error, "could not modify lua_script: entity is null\n");
        return 0;
    }

    if(lua_isboolean(L, 2)){
        ent->lua_script->active = lua_toboolean(L, 2);
    }

    if(lua_isstring(L, 3)){
        /*
            TODO: idk if this is a good way to handle this,
            or if we should even expose this variable at all,
            but lets just remove the script and recreate it with
            the new path for now.
        */
        ye_remove_lua_script_component(ent);
        ye_add_lua_script_component(ent, lua_tostring(L, 3), NULL);
    }

    return 0;
}



void ye_lua_lua_script_register(lua_State *L) {
    // init
    lua_register(L, "ye_lua_create_lua_script", ye_lua_create_lua_script);

    // query
    lua_register(L, "ye_lua_lua_script_query", ye_lua_lua_script_query);

    // modify
    lua_register(L, "ye_lua_lua_script_modify", ye_lua_lua_script_modify);
}