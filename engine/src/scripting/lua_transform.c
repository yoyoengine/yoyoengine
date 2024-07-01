/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <lua.h>

#include <yoyoengine/ecs/transform.h>
#include <yoyoengine/lua_api.h>
#include <yoyoengine/logging.h>

/*
    CONSTRUCT
*/

int ye_lua_create_transform(lua_State *L) {
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not create transform: entity is null\n");
        return 0;
    }

    // get x and y
    int x = luaL_checknumber(L, 2);
    int y = luaL_checknumber(L, 3);

    ye_add_transform_component(ent, x, y);

    return 0;
}

////////////



/*
    POSITION
*/

int ye_lua_transform_get_position_x(lua_State *L) {
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not get transform x: entity is null\n");
        return 0;
    }

    if(ent->transform == NULL) {
        ye_add_transform_component(ent, 0, 0);
    }

    lua_pushnumber(L, ent->transform->x);

    return 1;
}

int ye_lua_transform_get_position_y(lua_State *L) {
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not get transform y: entity is null\n");
        return 0;
    }

    if(ent->transform == NULL) {
        ye_add_transform_component(ent, 0, 0);
    }

    lua_pushnumber(L, ent->transform->y);

    return 1;
}

int ye_lua_transform_set_position_x(lua_State *L) {
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not set transform x: entity is null\n");
        return 0;
    }

    if(ent->transform == NULL) {
        ye_add_transform_component(ent, 0, 0);
    }

    int x = luaL_checknumber(L, 2);

    ent->transform->x = x;

    return 0;
}

int ye_lua_transform_set_position_y(lua_State *L) {
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not set transform y: entity is null\n");
        return 0;
    }

    if(ent->transform == NULL) {
        ye_add_transform_component(ent, 0, 0);
    }

    int y = luaL_checknumber(L, 2);

    ent->transform->y = y;

    return 0;
}

////////////



void ye_lua_transform_register(lua_State *L) {
    // init
    lua_register(L, "ye_lua_create_transform", ye_lua_create_transform);

    // position
    lua_register(L, "ye_lua_transform_get_position_x", ye_lua_transform_get_position_x);
    lua_register(L, "ye_lua_transform_get_position_y", ye_lua_transform_get_position_y);
    lua_register(L, "ye_lua_transform_set_position_x", ye_lua_transform_set_position_x);
    lua_register(L, "ye_lua_transform_set_position_y", ye_lua_transform_set_position_y);
}