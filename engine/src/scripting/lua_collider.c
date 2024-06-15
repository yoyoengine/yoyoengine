/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
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

#include <yoyoengine/ecs/collider.h>
#include <yoyoengine/lua_api.h>
#include <yoyoengine/logging.h>



int ye_lua_create_static_collider(lua_State *L) {
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not create static collider component: entity is null\n");
        return 0;
    }

    struct ye_rectf rect = {
        .x = luaL_checknumber(L, 2),
        .y = luaL_checknumber(L, 3),
        .w = luaL_checknumber(L, 4),
        .h = luaL_checknumber(L, 5)
    };

    ye_add_static_collider_component(ent, rect);

    return 0;
}

int ye_lua_create_trigger_collider(lua_State *L) {
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not create trigger collider component: entity is null\n");
        return 0;
    }

    struct ye_rectf rect = {
        .x = luaL_checknumber(L, 2),
        .y = luaL_checknumber(L, 3),
        .w = luaL_checknumber(L, 4),
        .h = luaL_checknumber(L, 5)
    };

    ye_add_trigger_collider_component(ent, rect);

    return 0;
}

int ye_lua_collider_query(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not query tag: entity is null\n");
        return 0;
    }

    lua_pushboolean(L, ent->collider->active);
    lua_pushboolean(L, ent->collider->relative);
    lua_pushnumber(L, ent->collider->rect.x);
    lua_pushnumber(L, ent->collider->rect.y);
    lua_pushnumber(L, ent->collider->rect.w);
    lua_pushnumber(L, ent->collider->rect.h);
    lua_pushboolean(L, ent->collider->is_trigger);

    return 7;
}

int ye_lua_collider_modify(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);
    
    if(ent == NULL) {
        ye_logf(error, "could not modify collider: entity is null\n");
        return 0;
    }

    if(lua_isboolean(L, 2)){
        ent->collider->active = lua_toboolean(L, 2);
    }

    if(lua_isboolean(L, 3)){
        ent->collider->relative = lua_toboolean(L, 3);
    }

    if(lua_isnumber(L, 4)){
        ent->collider->rect.x = lua_tonumber(L, 4);
    }

    if(lua_isnumber(L, 5)){
        ent->collider->rect.y = lua_tonumber(L, 5);
    }

    if(lua_isnumber(L, 6)){
        ent->collider->rect.w = lua_tonumber(L, 6);
    }

    if(lua_isnumber(L, 7)){
        ent->collider->rect.h = lua_tonumber(L, 7);
    }

    if(lua_isboolean(L, 8)){
        ent->collider->is_trigger = lua_toboolean(L, 8);
    }

    return 0;
}



void ye_lua_collider_register(lua_State *L) {
    // init
    lua_register(L, "ye_lua_create_trigger_collider", ye_lua_create_trigger_collider);
    lua_register(L, "ye_lua_create_static_collider", ye_lua_create_static_collider);

    // query
    lua_register(L, "ye_lua_collider_query", ye_lua_collider_query);

    // modify
    lua_register(L, "ye_lua_collider_modify", ye_lua_collider_modify);
}