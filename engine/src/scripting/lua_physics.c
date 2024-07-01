/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <lua.h>

#include <yoyoengine/ecs/physics.h>
#include <yoyoengine/lua_api.h>
#include <yoyoengine/logging.h>



int ye_lua_create_physics_component(lua_State *L) {
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not create physics component: entity is null\n");
        return 0;
    }

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    ye_add_physics_component(ent, x, y);

    return 0;
}

int ye_lua_physics_query(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not query physics component: entity is null\n");
        return 0;
    }

    lua_pushboolean(L, ent->physics->active);
    lua_pushnumber(L, ent->physics->velocity.x);
    lua_pushnumber(L, ent->physics->velocity.y);
    lua_pushnumber(L, ent->physics->rotational_velocity);

    return 4;
}

int ye_lua_physics_modify(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);
    
    if(ent == NULL) {
        ye_logf(error, "could not modify physics component: entity is null\n");
        return 0;
    }

    if(lua_isboolean(L, 2)){
        ent->physics->active = lua_toboolean(L, 2);
    }

    if(lua_isnumber(L, 3)){
        ent->physics->velocity.x = luaL_checknumber(L, 3);
    }

    if(lua_isnumber(L, 4)){
        ent->physics->velocity.y = luaL_checknumber(L, 4);
    }

    if(lua_isnumber(L, 5)){
        ent->physics->rotational_velocity = luaL_checknumber(L, 5);
    }

    return 0;
}



void ye_lua_physics_register(lua_State *L) {
    // init
    lua_register(L, "ye_lua_create_physics_component", ye_lua_create_physics_component);

    // query
    lua_register(L, "ye_lua_physics_query", ye_lua_physics_query);

    // modify
    lua_register(L, "ye_lua_physics_modify", ye_lua_physics_modify);
}