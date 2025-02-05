/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stdlib.h>

#include <SDL_net.h>

#include <yoyoengine/logging.h>

void ye_init_networking() {
    if (SDLNet_Init() < 0) {
        ye_logf(error,"Failed to initialize networking: %s\n", SDL_GetError());
        exit(1);
    }
    ye_logf(info,"Initialized networking.\n");
}

void ye_shutdown_networking() {
    // normal shutdown
    SDLNet_Quit();
    ye_logf(info,"Shut down networking.\n");
}