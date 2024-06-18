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

#include <stdbool.h>

#include <lua.h>

#include <yoyoengine/scene.h>
#include <yoyoengine/lua_api.h>
#include <yoyoengine/logging.h>

int ye_lua_load_scene(lua_State *L) {
    const char *scene_name = lua_tostring(L, 1);

    // defer to next frame to avoid breaking the lua stack
    ye_load_scene_deferred(scene_name);

    return 0;
}

int ye_lua_scene_register(lua_State *L) {
    lua_register(L, "ye_load_scene", ye_lua_load_scene);

    return 0;
}