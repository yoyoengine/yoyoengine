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
#include <stdlib.h>

#include <lua.h>

#include <yoyoengine/timer.h>
#include <yoyoengine/lua_api.h>
#include <yoyoengine/logging.h>

#define YE_LUA_MAX_TIMER_ARGS 10

struct ye_lua_timer_data {
    lua_State *L;
    int callback_ref;
    int arg_refs[YE_LUA_MAX_TIMER_ARGS];
    int num_args;
};

int ye_timer_lua_resolve(struct ye_timer * timer) {
    struct ye_lua_timer_data * lua_data = (struct ye_lua_timer_data *)timer->data;
    lua_rawgeti(lua_data->L, LUA_REGISTRYINDEX, lua_data->callback_ref);

    // Push the stored arguments back onto the stack
    for (int i = 0; i < lua_data->num_args; i++) {
        lua_rawgeti(lua_data->L, LUA_REGISTRYINDEX, lua_data->arg_refs[i]);
    }

    lua_call(lua_data->L, lua_data->num_args, 0);

    // cleanup (if this timer will not continue looping)
    if(timer->loops - 1 == 0) { // -1 this is before we decrement loops in engine
        // free callback ref
        luaL_unref(lua_data->L, LUA_REGISTRYINDEX, lua_data->callback_ref);
        
        // free varargs refs
        for (int i = 0; i < lua_data->num_args; i++) {
            luaL_unref(lua_data->L, LUA_REGISTRYINDEX, lua_data->arg_refs[i]);
        }

        free(lua_data);
    }
    // timer will be freed by engine

    return 0;
}

int ye_lua_create_timer(lua_State *L) {
    struct ye_timer * timer = malloc(sizeof(struct ye_timer));
    timer->length_ms = lua_tointeger(L, 1);

    // pack the lua function (arg 2) in a way we can resolve later
    struct ye_lua_timer_data * lua_data = malloc(sizeof(struct ye_lua_timer_data));
    lua_data->L = L;
    lua_pushvalue(L, 2);
    lua_data->callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    // Store varargs
    lua_data->num_args = lua_gettop(L) - 4; // excluding the first 4 args
    for (int i = 0; i < lua_data->num_args; i++) {
        lua_pushvalue(L, 5 + i);
        lua_data->arg_refs[i] = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    timer->data = lua_data;
    timer->loops = lua_tointeger(L, 3);
    timer->start_ticks = lua_tointeger(L, 4);
    timer->callback = ye_timer_lua_resolve;

    ye_register_timer(timer);

    return 0;
}

int ye_lua_timer_get_ticks(lua_State *L) {
    lua_pushinteger(L, SDL_GetTicks());
    return 1;
}

int ye_lua_timer_register(lua_State *L) {
    lua_register(L, "ye_lua_timer_create_timer", ye_lua_create_timer);

    lua_register(L, "ye_lua_timer_get_ticks", ye_lua_timer_get_ticks);

    return 0;
}