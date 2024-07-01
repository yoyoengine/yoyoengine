/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <lua.h>

#include <yoyoengine/ecs/tag.h>
#include <yoyoengine/lua_api.h>
#include <yoyoengine/logging.h>



int ye_lua_create_tag(lua_State *L) {
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not create tag component: entity is null\n");
        return 0;
    }

    ye_add_tag_component(ent);

    return 0;
}



int ye_lua_tag_query(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not query tag: entity is null\n");
        return 0;
    }

    lua_pushboolean(L, ent->tag->active);

    return 1;
}

int ye_lua_tag_modify(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);
    
    if(ent == NULL) {
        ye_logf(error, "could not modify tag: entity is null\n");
        return 0;
    }

    if(lua_isboolean(L, 2)){
        ent->tag->active = lua_toboolean(L, 2);
    }

    return 0;
}

int ye_lua_tag_has_tag(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);
    const char * tag = lua_tostring(L, 2);

    if(ent == NULL) {
        ye_logf(error, "could not check if entity has tag: entity is null\n");
        return 0;
    }

    lua_pushboolean(L, ye_entity_has_tag(ent, tag));
    return 1;
}

int ye_lua_tag_add_tag(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);
    const char * tag = lua_tostring(L, 2);

    if(ent == NULL) {
        ye_logf(error, "could not add tag: entity is null\n");
        return 0;
    }

    ye_add_tag(ent, tag);
    return 0;
}

int ye_lua_tag_remove_tag(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);
    const char * tag = lua_tostring(L, 2);

    if(ent == NULL) {
        ye_logf(error, "could not remove tag: entity is null\n");
        return 0;
    }

    ye_remove_tag(ent, tag);
    return 0;
}



void ye_lua_tag_register(lua_State *L) {
    // init
    lua_register(L, "ye_lua_create_tag", ye_lua_create_tag);

    // query
    lua_register(L, "ye_lua_tag_query", ye_lua_tag_query);

    // modify
    lua_register(L, "ye_lua_tag_modify", ye_lua_tag_modify);

    // tag methods
    lua_register(L, "ye_lua_tag_has_tag", ye_lua_tag_has_tag);
    lua_register(L, "ye_lua_tag_add_tag", ye_lua_tag_add_tag);
    lua_register(L, "ye_lua_tag_remove_tag", ye_lua_tag_remove_tag);
}