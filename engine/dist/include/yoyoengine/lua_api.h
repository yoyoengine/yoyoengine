/*
    This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

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

#ifndef LUA_API_H
#define LUA_API_H

#include <lua.h>
// #include <lua/lualib.h>
// #include <lua/lauxlib.h>
#include <yoyoengine/ecs/lua_script.h>

/**
 * @brief Takes in a lua state and registers the engine API with it.
 * 
 * @param state 
 */
void ye_register_lua_scripting_api(lua_State *state);

/*
    Callbacks (lua_api_callbacks.c)
*/
void ye_run_lua_on_mount(struct ye_component_lua_script *script);
void ye_run_lua_on_unmount(struct ye_component_lua_script *script);
void ye_run_lua_on_update(struct ye_component_lua_script *script);

//////////////////////////////////////////////////////////////////////////////

/*
    Entity
*/
void ye_lua_entity_register(lua_State *L);

//////////////////////////////////////////////////////////////////////////////

#endif