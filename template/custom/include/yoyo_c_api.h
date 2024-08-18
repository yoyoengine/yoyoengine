/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/*
    This file used to be a bloated hacky mess,
    but look how clean it is now! :)
*/
#include <yoyoengine/yoyoengine.h>

/*
    Controls whether the game is running.
    Set this to false at any point and everything will quit.
*/
extern bool YG_RUNNING;

/*
    Entry point to user scripting.
*/
void yoyo_register_callbacks(void);