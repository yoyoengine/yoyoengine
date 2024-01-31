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

/**
 * @file lua_script.h
 * @brief lua script component
 */

#ifndef YE_LUA_SCRIPT_H
#define YE_LUA_SCRIPT_H

#include <yoyoengine/yoyoengine.h>

#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>

/**
 * @brief The script component for lua. Will recieve callbacks from the engine.
 */
struct ye_component_lua_script {
    bool active;                    // controls whether system will act upon this component
    char *script_handle;            // the path to the script

    lua_State *state;               // the lua state for this script

    /*
        Once the script is boostrapped, we parse references so we know
        not to look for invalid references.
    */
    bool has_on_mount;
    bool has_on_unmount;
    bool has_on_update;
    // ... etc
};

/**
 * @brief Add a lua script component to an entity
 * 
 * @param entity The target entity
 * @param script_path The path to the script
 */
bool ye_add_lua_script_component(struct ye_entity *entity, char *handle);

/**
 * @brief Remove a lua script component from an entity
 * 
 * @param entity The target entity
 */
void ye_remove_lua_script_component(struct ye_entity *entity);

/**
 * @brief The system that controls the behavior of lua scripts
 */
void ye_system_lua_scripting();

#endif