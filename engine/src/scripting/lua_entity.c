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

#include <stdbool.h>

#include <lua.h>

#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/lua_api.h>
#include <yoyoengine/logging.h>

/*
    CONSTRUCT
*/

int ye_lua_ent_get_entity_named(lua_State *L) {
    const char *name = lua_tostring(L, 1);
    struct ye_entity *entity = ye_get_entity_by_name(name);

    if(entity != NULL)
        lua_pushlightuserdata(L, entity);
    else
        lua_pushnil(L);
    return 1;
}

int ye_lua_create_entity(lua_State *L) {
    int nargs = lua_gettop(L);

    struct ye_entity * ent = NULL;

    if(nargs == 0) {
        ent = ye_create_entity();
    }
    else if(nargs == 1) {
        const char *name = lua_tostring(L, 1);
        ent = ye_create_entity_named(name);
    }

    if(ent != NULL)
        lua_pushlightuserdata(L, ent);
    else
        lua_pushnil(L);

    return 1;
}

////////////



/*
    ACTIVE
*/

int ye_lua_ent_set_active(lua_State *L) {
    struct ye_entity *entity = lua_touserdata(L, 1);
    bool active = lua_toboolean(L, 2);
    
    if(entity == NULL) {
        ye_logf(error, "could not set active state: entity is null\n");
        return 0;
    }

    entity->active = active;
    return 0;
}

int ye_lua_ent_get_active(lua_State *L) {
    struct ye_entity *entity = lua_touserdata(L, 1);

    if(entity == NULL) {
        ye_logf(error, "could not get active state: entity is null\n");
        lua_pushboolean(L, false);
        return 1;
    }

    lua_pushboolean(L, entity->active);
    return 1;
}

////////////



/*
    ID
*/

int ye_lua_ent_get_id(lua_State *L) {
    struct ye_entity *entity = lua_touserdata(L, 1);

    if(entity == NULL) {
        ye_logf(error, "could not get entity ID: entity is null\n");
        lua_pushinteger(L, -1);
        return 1;
    }

    lua_pushinteger(L, entity->id);
    return 1;
}

//////////



/*
    NAME
*/

int ye_lua_ent_get_name(lua_State *L) {
    struct ye_entity *entity = lua_touserdata(L, 1);

    if(entity == NULL) {
        ye_logf(error, "could not get entity name: entity is null\n");
        lua_pushstring(L, "ERROR");
        return 1;
    }

    lua_pushstring(L, entity->name);
    return 1;
}

int ye_lua_ent_set_name(lua_State *L) {
    struct ye_entity *entity = lua_touserdata(L, 1);
    const char *name = lua_tostring(L, 2);

    if(entity == NULL) {
        ye_logf(error, "could not set entity name: entity is null\n");
        return 0;
    }

    ye_rename_entity(entity, name);
    return 0;
}

//////////



void ye_lua_entity_register(lua_State *L) {
    // init
    lua_register(L, "ye_lua_ent_get_entity_named", ye_lua_ent_get_entity_named);
    lua_register(L, "ye_lua_create_entity", ye_lua_create_entity);

    // active
    lua_register(L, "ye_lua_ent_get_active", ye_lua_ent_get_active);
    lua_register(L, "ye_lua_ent_set_active", ye_lua_ent_set_active);

    // id
    lua_register(L, "ye_lua_ent_get_id", ye_lua_ent_get_id);

    // name
    lua_register(L, "ye_lua_ent_get_name", ye_lua_ent_get_name);
    lua_register(L, "ye_lua_ent_set_name", ye_lua_ent_set_name);
}