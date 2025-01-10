/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <lua.h>

#include <yoyoengine/lua_api.h>
#include <yoyoengine/logging.h>



int ye_lua_create_rigidbody_component(lua_State *L) {
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not create rigidbody component: entity is null\n");
        return 0;
    }

    // float x = luaL_checknumber(L, 2);
    // float y = luaL_checknumber(L, 3);

    // ye_add_rigidbody_component(ent, x, y);

    // TODO: RIGIDBODY

    return 0;
}

int ye_lua_rigidbody_query(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not query rigidbody component: entity is null\n");
        return 0;
    }

    // RB TODO
    // lua_pushboolean(L, ent->rigidbody->active);
    // lua_pushnumber(L, ent->rigidbody->velocity.x);
    // lua_pushnumber(L, ent->rigidbody->velocity.y);
    // lua_pushnumber(L, ent->rigidbody->rotational_velocity);

    return 4;
}

int ye_lua_rigidbody_modify(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);
    
    if(ent == NULL) {
        ye_logf(error, "could not modify rigidbody component: entity is null\n");
        return 0;
    }

    // TODO RIGIDBODY

    return 0;
}



void ye_lua_rigidbody_register(lua_State *L) {
    // init
    lua_register(L, "ye_lua_create_rigidbody_component", ye_lua_create_rigidbody_component);

    // query
    lua_register(L, "ye_lua_rigidbody_query", ye_lua_rigidbody_query);

    // modify
    lua_register(L, "ye_lua_rigidbody_modify", ye_lua_rigidbody_modify);
}