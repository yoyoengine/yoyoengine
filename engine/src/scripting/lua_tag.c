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