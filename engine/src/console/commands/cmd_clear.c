/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <string.h>

#include <yoyoengine/console.h>
#include <yoyoengine/logging.h>
#include <yoyoengine/commands.h>

void _clear_usage() {
    ye_logf(_YE_RESERVED_LL_SYSTEM, "Usage:\n");
    ye_logf(_YE_RESERVED_LL_SYSTEM, "       clear : clear the developer console output.\n");
}

void ye_cmd_clear(int argc, const char **argv) {
    (void)argv;

    if(argc < 0){
        _clear_usage();
        return;
    }

    if(argc > 0)
        ye_logf(_YE_RESERVED_LL_SYSTEM, "warning: too many args (%d). Ignoring argc>0.\n",argc);

    ye_console_clear();
}