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

int ye_lua_ent_set_active(lua_State *L) {
    struct ye_entity *entity = lua_touserdata(L, 1);
    bool active = lua_toboolean(L, 2);
    
    if(entity == NULL) {
        lua_pushstring(L, "entity is null"); // consider if keeping this, its vague in stdout
        lua_error(L);
    }
    entity->active = active;
    return 0;
}

int ye_lua_ent_get_entity_named(lua_State *L) {
    const char *name = lua_tostring(L, 1);
    struct ye_entity *entity = ye_get_entity_by_name(name);

    if(entity != NULL)
        lua_pushlightuserdata(L, entity);
    else
        lua_pushnil(L);
    return 1;
}

void ye_lua_entity_register(lua_State *L) {
    lua_register(L, "ye_lua_ent_get_entity_named", ye_lua_ent_get_entity_named);

    lua_register(L, "ye_lua_ent_set_active", ye_lua_ent_set_active);
}