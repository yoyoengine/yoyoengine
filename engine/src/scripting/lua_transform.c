/*
    This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
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