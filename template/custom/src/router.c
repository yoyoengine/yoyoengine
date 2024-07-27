/*
    This file is derived from a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <yoyoengine/yoyoengine.h>

#include "yoyo_c_api.h"

/*
    If you are planning on writing C scripts, you will likely be overriding some of the
    engine callbacks. I would reccomend populating this file with those stubs, and routing
    them to the correct callback handlers depending on the game state.

    For example, if you have a scene where you want to handle player input, and one where you
    do not, you could have a stub like this:

    enum YOUR_GAME_STATE {
        GAME,
        MENU
    } YOUR_GAME_STATE;

    // (dont forget to uncomment the proper macro in yoyo_c_api.h)
    void yoyo_handle_input(SDL_Event event) {
        if (YOUR_GAME_STATE == GAME) {
            game_handle_input(event);
        } else if (YOUR_GAME_STATE == MENU) {
            menu_handle_input(event);
        }
    }

    Happy Coding!
*/