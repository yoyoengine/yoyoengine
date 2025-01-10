/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
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

// TODO; this guy is almost useless now, idk, reexamine this
int ye_invoke_lua_function(lua_State* L, const char* functionName, ...) {
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

int ye_invoke_cross_state_function(lua_State* L) {
    
    struct ye_entity * target = lua_touserdata(L, 1);

    // validate target exists
    if(target == NULL) {
        ye_logf(error, "Attempting to invoke cross-state function on NULL entity.\n");
        return 0;
    }

    const char* functionName = lua_tostring(L, 2);
    lua_State* targetState = target->lua_script->state;

    // validate the target function exists
    if(!lua_getglobal(targetState, functionName)) {
        ye_logf(error, "Attempting to invoke cross-state function whose signature does not exist.\n");
        return 0;
    }

    // Number of arguments to pass
    int numArgs = lua_gettop(L) - 2;

    // Transfer arguments from the source state to the target state
    for (int i = 0; i < numArgs; i++) {
        lua_pushvalue(L, 3 + i);          // Push argument from source state
        lua_xmove(L, targetState, 1);     // Move it to the target state
    }

    // Call the function on the target state with numArgs arguments and capture return values
    int numRetVals = 0; // Number of return values to expect, can be customized
    if (lua_pcall(targetState, numArgs, LUA_MULTRET, 0) != LUA_OK) {
        // Handle the error
        const char* errorMsg = lua_tostring(targetState, -1);
        ye_logf(error, "Error invoking cross-state function: %s\n", errorMsg);
        lua_pop(targetState, 1); // Remove error message from the stack
        return 0;
    }

    // Get the number of return values
    numRetVals = lua_gettop(targetState);

    // Move the return values back to the source state
    for (int i = 0; i < numRetVals; i++) {
        lua_pushvalue(targetState, -(numRetVals - i)); // Push return value to the target stack
        lua_xmove(targetState, L, 1);                  // Move it to the source state
    }

    // Cleanup the target state stack
    lua_pop(targetState, numRetVals);

    // Return the number of return values to the source Lua state
    return numRetVals;
}

int ye_read_cross_state_value(lua_State* L) {
    struct ye_entity * target = lua_touserdata(L, 1);

    // validate target exists
    if(target == NULL) {
        ye_logf(error, "Attempting to read cross-state value on NULL entity.\n");
        return 0;
    }

    const char* valueName = lua_tostring(L, 2);
    lua_State* targetState = target->lua_script->state;

    // validate the target value exists
    if(!lua_getglobal(targetState, valueName)) {
        ye_logf(error, "Attempting to read cross-state value that does not exist.\n");
        return 0;
    }

    // Transfer the value from the target state to the source state
    lua_xmove(targetState, L, 1);

    // Return the number of return values to the source Lua state
    return 1;
}

int ye_write_cross_state_value(lua_State* L) {
    // Retrieve the target entity from the first argument
    struct ye_entity *target = lua_touserdata(L, 1);

    // Validate target exists
    if (target == NULL) {
        ye_logf(error, "Attempting to write cross-state variable on NULL entity.\n");
        return 0;
    }

    // Retrieve the variable name from the second argument
    const char* varName = lua_tostring(L, 2);

    // Get the target Lua state from the entity
    lua_State* targetState = target->lua_script->state;

    // Move the new value from the source state to the target state
    lua_pushvalue(L, 3);
    lua_xmove(L, targetState, 1);

    // Set the global variable in the target state
    lua_setglobal(targetState, varName);

    return 0;
}

void ye_run_lua_on_mount(struct ye_component_lua_script *script) {
    if(script->has_on_mount) {
        ye_invoke_lua_function(script->state, "onMount", LUA_END_ARGS);
    }
}

void ye_run_lua_on_unmount(struct ye_component_lua_script *script) {
    if(script == NULL) {
        // printf("script is null\n");
        return;
    }
    
    if(script->has_on_unmount) {
        ye_invoke_lua_function(script->state, "onUnmount", LUA_END_ARGS);
    }
}

void ye_run_lua_on_update(struct ye_component_lua_script *script) {
    if(script->has_on_update) {
        ye_invoke_lua_function(script->state, "onUpdate", LUA_END_ARGS);
    }
}

// Function to create an entity table in Lua
void create_entity_table(lua_State *L, void *entity) {
    lua_newtable(L); // Create a new table

    // Set the _c_entity field
    lua_pushlightuserdata(L, entity);
    lua_setfield(L, -2, "_c_entity");

    // Get the Entity_mt table from the global state
    lua_getglobal(L, "Entity_mt");
    if (!lua_istable(L, -1)) {
        // Handle the error (Entity_mt is not a table)
        lua_pop(L, 1);  // Remove the non-table value
        printf("Error: Entity_mt is not a table!\n");
    } else {
        lua_setmetatable(L, -2);  // Set the table as the metatable
    }
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

// super useful for debugging :D
// void dump_globals(lua_State* L) {
//     // Push the global table onto the stack
//     lua_pushglobaltable(L);
    
//     // Start iterating through the global table
//     lua_pushnil(L);  // First key (nil means start from the beginning)
    
//     printf("Dumping globals:\n");
    
//     while (lua_next(L, -2) != 0) {
//         // key is at index -2 and value is at index -1
//         if (lua_type(L, -2) == LUA_TSTRING) {
//             printf("%s = ", lua_tostring(L, -2));
//         } else {
//             printf("[non-string key] = ");
//         }

//         // Print value based on its type
//         switch (lua_type(L, -1)) {
//             case LUA_TNUMBER:
//                 printf("%f\n", lua_tonumber(L, -1));
//                 break;
//             case LUA_TSTRING:
//                 printf("%s\n", lua_tostring(L, -1));
//                 break;
//             case LUA_TBOOLEAN:
//                 printf("%s\n", lua_toboolean(L, -1) ? "true" : "false");
//                 break;
//             default:
//                 printf("%s\n", lua_typename(L, lua_type(L, -1)));
//                 break;
//         }

//         // Remove value, keep key for the next iteration
//         lua_pop(L, 1);
//     }
    
//     // Remove the global table from the stack
//     lua_pop(L, 1);
// }

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