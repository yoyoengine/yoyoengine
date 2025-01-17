/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef COMMANDS_H
#define COMMANDS_H

#include <yoyoengine/export.h>

#include <yoyoengine/console.h>

/*
    shutdown engine

    TODO:
    show/hide debug panels which are presets
*/

/*
    IMPORTANT:
    commands are _expected_ to implement a case for argc = -1 which will print usage information.
    
    this is not enforcable by the engine, but you should do it!!!!!

    the engine allows the "help" command to invoke "help [your command]" and trigger this case!
*/

YE_API void ye_cmd_help(int argc, const char **argv);

YE_API void ye_cmd_entlist(int argc, const char **argv);

YE_API void ye_cmd_scene(int argc, const char **argv);

YE_API void ye_cmd_config(int argc, const char **argv);

YE_API void ye_cmd_quit(int argc, const char **argv);

YE_API void ye_cmd_clear(int argc, const char **argv);

YE_API void ye_cmd_overlay(int argc, const char **argv);

#endif