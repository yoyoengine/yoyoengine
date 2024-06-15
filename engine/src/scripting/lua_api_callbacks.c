/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
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

#include <stdbool.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <yoyoengine/logging.h>
#include <yoyoengine/ecs/lua_script.h>

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
        callLuaFunction(script->state, "onMount", LUA_END_ARGS);
    }
}

void ye_run_lua_on_unmount(struct ye_component_lua_script *script) {
    if(script == NULL) {
        // printf("script is null\n");
        return;
    }
    
    if(script->has_on_unmount) {
        callLuaFunction(script->state, "onUnmount", LUA_END_ARGS);
    }
}

void ye_run_lua_on_update(struct ye_component_lua_script *script) {
    if(script->has_on_update) {
        callLuaFunction(script->state, "onUpdate", LUA_END_ARGS);
    }
}

// Function to create an entity table in Lua
void create_entity_table(lua_State *L, void *entity) {
    lua_newtable(L); // Create a new table

    // Set the _c_entity field
    lua_pushlightuserdata(L, entity);
    lua_setfield(L, -2, "_c_entity");

    // Set the metatable
    luaL_getmetatable(L, "Entity_mt");
    lua_setmetatable(L, -2);
}

// Function to print the Lua stack for debugging
void print_lua_stack(lua_State *L) {
    int top = lua_gettop(L);
    for (int i = 1; i <= top; i++) {
        int t = lua_type(L, i);
        switch (t) {
            case LUA_TSTRING:
                printf("'%s'", lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:
                printf(lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                printf("%g", lua_tonumber(L, i));
                break;
            default:
                printf("%s", lua_typename(L, t));
                break;
        }
        printf("  ");
    }
    printf("\n");
}

void ye_run_lua_on_collision(struct ye_component_lua_script *script, struct ye_entity *entity1, struct ye_entity *entity2) {
    if(script->has_on_collision) {
        // get the onCollision function
        lua_getglobal(script->state, "onCollision");
        
        // Check if the onCollision function exists and is callable
        if (!lua_isfunction(script->state, -1)) {
            ye_logf(error, "Invalid onCollision Lua function reference.\n");
            lua_pop(script->state, 1);
            return;
        }

        // Create and push the entity1 table
        create_entity_table(script->state, entity1);

        // Create and push the entity2 table
        create_entity_table(script->state, entity2);

        // Call onCollision with the two entities
        if (lua_pcall(script->state, 2, 0, 0) != LUA_OK) {
            ye_logf(error, "Error calling function: %s\n", lua_tostring(script->state, -1));
            lua_pop(script->state, 1); // Pop the error message
        }
    }
}

void ye_run_lua_on_trigger_enter(struct ye_component_lua_script *script, struct ye_entity *entity1, struct ye_entity *entity2) {
    if(script->has_on_trigger_enter) {
        // get the onTriggerEnter function
        lua_getglobal(script->state, "onTriggerEnter");
        
        // Check if the onTriggerEnter function exists and is callable
        if (!lua_isfunction(script->state, -1)) {
            ye_logf(error, "Invalid onTriggerEnter Lua function reference.\n");
            lua_pop(script->state, 1);
            return;
        }

        // Create and push the entity1 table
        create_entity_table(script->state, entity1);

        // Create and push the entity2 table
        create_entity_table(script->state, entity2);

        // Call onTriggerEnter with the two entities
        if (lua_pcall(script->state, 2, 0, 0) != LUA_OK) {
            ye_logf(error, "Error calling function: %s\n", lua_tostring(script->state, -1));
            lua_pop(script->state, 1); // Pop the error message
        }
    }
}