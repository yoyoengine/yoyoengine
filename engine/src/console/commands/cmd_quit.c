/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stdlib.h>
#include <string.h>

#include <yoyoengine/engine.h>
#include <yoyoengine/logging.h>
#include <yoyoengine/commands.h>

void _quit_usage() {
    ye_logf(_YE_RESERVED_LL_SYSTEM, "Usage:\n");
    ye_logf(_YE_RESERVED_LL_SYSTEM, "       quit : shutdown the engine (ungracefully, because the game cannot close itself).\n");
}

void ye_cmd_quit(int argc, const char **argv) {
    (void)argv;

    if(argc < 0){
        _quit_usage();
        return;
    }
    
    ye_shutdown_engine();
    exit(0);
}