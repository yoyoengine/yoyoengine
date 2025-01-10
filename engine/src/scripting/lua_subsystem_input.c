/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stdbool.h>

#include <SDL.h>
#include <lua.h>

#include <yoyoengine/utils.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/input.h>
#include <yoyoengine/lua_api.h>
#include <yoyoengine/logging.h>

/*
    Return a table with the mouse state
    .x: The x position of the mouse
    .y: The y position of the mouse
    .worldX: The x position of the mouse in world coordinates
    .worldY: The y position of the mouse in world coordinates
    .leftClicked: Whether the left mouse button is clicked
    .middleClicked: Whether the middle mouse button is clicked
    .rightClicked: Whether the right mouse button is clicked
*/
int ye_lua_query_mouse_state(lua_State *L) {
    int x, y;
    int buttons = SDL_GetMouseState(&x, &y);

    int wx = x; int wy = y;
    ye_get_mouse_world_position(&wx, &wy);

    lua_newtable(L);

    // x
    lua_pushinteger(L, x); lua_setfield(L, -2, "x");

    // y
    lua_pushinteger(L, y); lua_setfield(L, -2, "y");

    // world x
    lua_pushinteger(L, wx); lua_setfield(L, -2, "worldX");

    // world y
    lua_pushinteger(L, wy); lua_setfield(L, -2, "worldY");

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

/*
    Scope these mappings to this file to avoid excessive
    stack usage when querying controller state
*/
const char* button_names[] = {
    "a",            "b",                "x",            "y",
    "back",         "guide",            "start",
    "leftStick",    "rightStick",
    "leftShoulder", "rightShoulder",
    "dPadUp",       "dPadDown",         "dPadLeft",    "dPadRight"
    // triggers are axis, and as such are omitted here
};
SDL_GameControllerButton button_values[] = {
    SDL_CONTROLLER_BUTTON_A,            SDL_CONTROLLER_BUTTON_B,                SDL_CONTROLLER_BUTTON_X,            SDL_CONTROLLER_BUTTON_Y,
    SDL_CONTROLLER_BUTTON_BACK,         SDL_CONTROLLER_BUTTON_GUIDE,            SDL_CONTROLLER_BUTTON_START,
    SDL_CONTROLLER_BUTTON_LEFTSTICK,    SDL_CONTROLLER_BUTTON_RIGHTSTICK,
    SDL_CONTROLLER_BUTTON_LEFTSHOULDER, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
    SDL_CONTROLLER_BUTTON_DPAD_UP,      SDL_CONTROLLER_BUTTON_DPAD_DOWN,        SDL_CONTROLLER_BUTTON_DPAD_LEFT,    SDL_CONTROLLER_BUTTON_DPAD_RIGHT
};

int ye_lua_query_controller_state(lua_State *L) {
    // get the controller lua is curious about
    SDL_GameController *controller = YE_STATE.runtime.controllers[lua_tointeger(L, 1)];

    // if there is no controller, return nil
    if (controller == NULL) {
        lua_pushnil(L);
        return 1;
    }

    lua_newtable(L);

    // Automatically map button values to their names
    for (int i = 0; i < (int)(sizeof(button_names)/sizeof(button_names[0])); ++i) {
        lua_pushstring(L, button_names[i]);
        lua_pushboolean(L, SDL_GameControllerGetButton(controller, button_values[i]));
        lua_settable(L, -3);
    }

    // get the axis values (normalize each by sint16 max to get a -1.0 to 1.0 scale)
    lua_pushstring(L, "leftStickX");
    lua_pushnumber(L, SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f);
    lua_settable(L, -3);

    lua_pushstring(L, "leftStickY");
    lua_pushnumber(L, SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f);
    lua_settable(L, -3);

    lua_pushstring(L, "rightStickX");
    lua_pushnumber(L, SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX) / 32767.0f);
    lua_settable(L, -3);

    lua_pushstring(L, "rightStickY");
    lua_pushnumber(L, SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY) / 32767.0f);
    lua_settable(L, -3);

    lua_pushstring(L, "leftTrigger");
    lua_pushnumber(L, SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 32767.0f);
    lua_settable(L, -3);

    lua_pushstring(L, "rightTrigger");
    lua_pushnumber(L, SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 32767.0f);
    lua_settable(L, -3);

    // info on vendor, model, misc
    lua_pushstring(L, "name");
    lua_pushstring(L, SDL_GameControllerName(controller));
    lua_settable(L, -3);

    lua_pushstring(L, "attached");
    lua_pushboolean(L, SDL_GameControllerGetAttached(controller));
    lua_settable(L, -3);

    // lua_pushstring(L, "mapping");
    // lua_pushstring(L, SDL_GameControllerMapping(controller));
    // lua_settable(L, -3);

    return 1;
}

int ye_get_number_of_controllers(lua_State *L) {
    lua_pushinteger(L, YE_STATE.runtime.num_controllers);
    return 1;
}

int ye_lua_input_register(lua_State *L) {
    lua_register(L, "ye_lua_input_query_mouse", ye_lua_query_mouse_state);
    lua_register(L, "ye_lua_input_query_key", ye_lua_query_key_state);
    lua_register(L, "ye_lua_input_query_mod", ye_lua_input_query_key_mod);
    lua_register(L, "ye_lua_input_query_controller", ye_lua_query_controller_state);
    lua_register(L, "ye_lua_input_number_of_controllers", ye_get_number_of_controllers);

    return 0;
}