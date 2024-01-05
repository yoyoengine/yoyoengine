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

#define LUA_END_ARGS -2

int callLuaFunction(lua_State* L, const char* functionName, ...) {
    va_list args;
    int nargs = 0;

    lua_getglobal(L, functionName); // Get the function by name

    // Push arguments onto the Lua stack
    va_start(args, functionName);
    while (va_arg(args, int) != LUA_END_ARGS) {
        nargs++;
        switch (va_arg(args, int)) {
            case LUA_TSTRING:
                lua_pushstring(L, va_arg(args, const char*));
                break;
            case LUA_TNUMBER:
                lua_pushnumber(L, va_arg(args, double));
                break;
            // Handle other types as needed
        }
    }
    va_end(args);

    // Make the function call with nargs arguments
    if (lua_pcall(L, nargs, 0, 0) != LUA_OK) {
        fprintf(stderr, "Error calling function: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1); // Pop the error message
        return -1;
    }

    return 0;
}

void ye_run_lua_on_mount(struct ye_component_lua_script *script) {
    if(script->has_on_mount) {
        callLuaFunction(script->state, "on_mount", LUA_END_ARGS);
    }
}

void ye_run_lua_on_unmount(struct ye_component_lua_script *script) {
    if(script == NULL) {
        printf("script is null\n");
        return;
    }
    
    if(script->has_on_unmount) {
        callLuaFunction(script->state, "on_unmount", LUA_END_ARGS);
    }
}

void ye_run_lua_on_update(struct ye_component_lua_script *script) {
    if(script->has_on_update) {
        callLuaFunction(script->state, "on_update", LUA_END_ARGS);
    }
}