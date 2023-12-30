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

#include <yoyoengine/yoyoengine.h>

bool ye_run_lua_callback(struct ye_component_lua_script *script, int callback_ref, const char *callback_name) {
    lua_State *L = script->state;

    if (callback_ref != LUA_NOREF) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, callback_ref);

        if (!lua_isfunction(L, -1)) {
            ye_logf(error,"Invalid %s Lua function reference.\n", callback_name);
            lua_pop(L, 1);
            return false;
        }

        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            const char *e = lua_tostring(L, -1);
            ye_logf(error,"Error running %s function: %s\n", callback_name, e);
            lua_pop(L, 1);
            return false;
        }

        return true;
    } else {
        ye_logf(error,"%s function reference is not defined in the Lua script.\n", callback_name);
        return false;
    }
}

bool ye_run_lua_on_mount(struct ye_component_lua_script *script) {
    return ye_run_lua_callback(script, script->on_mount, "on_mount");
}

bool ye_run_lua_on_unmount(struct ye_component_lua_script *script) {
    return ye_run_lua_callback(script, script->on_unmount, "on_unmount");
}

bool ye_run_lua_on_update(struct ye_component_lua_script *script) {
    return ye_run_lua_callback(script, script->on_update, "on_update");
}