/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
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

int ye_lua_get_entity_by_id(lua_State *L) {
    int id = lua_tointeger(L, 1);
    struct ye_entity *entity = ye_get_entity_by_id(id);

    if(entity != NULL)
        lua_pushlightuserdata(L, entity);
    else
        lua_pushnil(L);
    return 1;
}

int ye_lua_get_entity_by_tag(lua_State *L) {
    const char *tag = lua_tostring(L, 1);
    struct ye_entity *entity = ye_get_entity_by_tag(tag);

    if(entity != NULL)
        lua_pushlightuserdata(L, entity);
    else
        lua_pushnil(L);
    return 1;
}

int ye_lua_delete_entity(lua_State *L) {
    struct ye_entity *entity = lua_touserdata(L, 1);

    if(entity == NULL) {
        ye_logf(error, "could not delete entity: entity is null\n");
        return 0;
    }

    ye_destroy_entity(entity);
    return 0;
}

int ye_lua_duplicate_entity(lua_State *L) {
    struct ye_entity *entity = lua_touserdata(L, 1);
    struct ye_entity *new_entity = ye_duplicate_entity(entity);

    if(new_entity != NULL)
        lua_pushlightuserdata(L, new_entity);
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
    lua_register(L, "ye_lua_ent_get_entity_by_id", ye_lua_get_entity_by_id);
    lua_register(L, "ye_lua_ent_get_entity_by_tag", ye_lua_get_entity_by_tag);
    lua_register(L, "ye_lua_create_entity", ye_lua_create_entity);

    // active
    lua_register(L, "ye_lua_ent_get_active", ye_lua_ent_get_active);
    lua_register(L, "ye_lua_ent_set_active", ye_lua_ent_set_active);

    // id
    lua_register(L, "ye_lua_ent_get_id", ye_lua_ent_get_id);

    // name
    lua_register(L, "ye_lua_ent_get_name", ye_lua_ent_get_name);
    lua_register(L, "ye_lua_ent_set_name", ye_lua_ent_set_name);

    // misc
    lua_register(L, "ye_lua_delete_entity", ye_lua_delete_entity);
    lua_register(L, "ye_lua_duplicate_entity", ye_lua_duplicate_entity);
}