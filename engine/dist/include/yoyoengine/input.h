/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/*
    Initializes the input system.

    Will setup connections to a game controller if detected.
    NOTE: we only currently support one controller. It is relatively trivial
    to add support for multiple, but I'm leaving that for future me.
*/
void ye_init_input();

void ye_system_input();

void ye_shutdown_input();