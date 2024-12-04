/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/*
    PHYSICS SYSTEM V2:
    A simple velocity (immediate impulse application) physics system.

    FUTURE WISHLIST:
    - physics thread
    - advanced paramters (drag, static friction, etc)
*/

#ifndef YE_TAR_H
#define YE_TAR_H

#include <yoyoengine/export.h>

YE_API void ye_physics_tick(float dt);

#endif // YE_TAR_H