/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YE_INPUT_H
#define YE_INPUT_H

#include <yoyoengine/export.h>

/*
    Initializes the input system.

    Will setup connections to a game controller if detected.
    NOTE: we only currently support one controller. It is relatively trivial
    to add support for multiple, but I'm leaving that for future me.
*/
YE_API void ye_init_input();

YE_API void ye_system_input();

YE_API void ye_shutdown_input();

#endif