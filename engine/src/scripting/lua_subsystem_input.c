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

#include <SDL2/SDL.h>
#include <lua.h>

#include <yoyoengine/engine.h>
#include <yoyoengine/input.h>
#include <yoyoengine/lua_api.h>
#include <yoyoengine/logging.h>

/*
    Return a table with the mouse state
    .x: The x position of the mouse
    .y: The y position of the mouse
    .leftClicked: Whether the left mouse button is clicked
    .middleClicked: Whether the middle mouse button is clicked
    .rightClicked: Whether the right mouse button is clicked
*/
int ye_lua_query_mouse_state(lua_State *L) {
    int x, y;
    int buttons = SDL_GetMouseState(&x, &y);

    lua_newtable(L);

    // x
    lua_pushinteger(L, x); lua_setfield(L, -2, "x");

    // y
    lua_pushinteger(L, y); lua_setfield(L, -2, "y");

    // leftClicked
    lua_pushboolean(L, buttons & SDL_BUTTON(SDL_BUTTON_LEFT)); lua_setfield(L, -2, "leftClicked");

    // middleClicked
    lua_pushboolean(L, buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)); lua_setfield(L, -2, "middleClicked");

    // rightClicked
    lua_pushboolean(L, buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)); lua_setfield(L, -2, "rightClicked");

    return 1;
}

int ye_lua_query_key_state(lua_State *L) {
    // true means we use keycodes, false means scancodes
    bool using_key = lua_toboolean(L, 1);
    int code = lua_tointeger(L, 2); // the code passed

    // get keyboard state
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (using_key) {
        SDL_Keycode key = code;

        lua_pushboolean(L, state[SDL_GetScancodeFromKey(key)]);
    } else {
        SDL_Scancode key = code;

        lua_pushboolean(L, SDL_GetKeyboardState(NULL)[key]);
    }

    return 1;
}

int ye_lua_input_query_key_mod(lua_State *L) {

    int mod = lua_tointeger(L, 1);

    lua_pushboolean(L, SDL_GetModState() & mod);

    return 1;
}

int ye_lua_input_register(lua_State *L) {
    lua_register(L, "ye_lua_input_query_mouse", ye_lua_query_mouse_state);
    lua_register(L, "ye_lua_input_query_key", ye_lua_query_key_state);
    lua_register(L, "ye_lua_input_query_mod", ye_lua_input_query_key_mod);

    return 0;
}