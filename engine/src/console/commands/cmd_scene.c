/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <string.h>

#include <yoyoengine/scene.h>
#include <yoyoengine/logging.h>
#include <yoyoengine/commands.h>

void _scene_usage() {
    ye_logf(_YE_RESERVED_LL_SYSTEM, "Usage:\n");
    ye_logf(_YE_RESERVED_LL_SYSTEM, "       scene reload : reload the active scene\n");
    ye_logf(_YE_RESERVED_LL_SYSTEM, "       scene load [scene handle] : load a new scene by handle\n");
}

void ye_cmd_scene(int argc, const char **argv) {
    if(argc <= 0){
        _scene_usage();
        return;
    }
    
    if(strcmp(argv[0], "reload") == 0){
        if(argc > 1)
            ye_logf(_YE_RESERVED_LL_SYSTEM, "warning: too many args (%d). Ignoring argc>1.\n",argc);

        ye_logf(_YE_RESERVED_LL_SYSTEM, "Reloading scene...\n");
        ye_reload_scene();
        return;
    }

    if(strcmp(argv[0], "load") == 0){
        if(argc < 2){
            ye_logf(_YE_RESERVED_LL_SYSTEM, "Error: did not specify a scene to load.\n");
            _scene_usage();
            return;
        }

        if(argc > 2)
            ye_logf(_YE_RESERVED_LL_SYSTEM, "warning: too many args (%d). Ignoring argc>2.\n",argc);

        ye_logf(_YE_RESERVED_LL_SYSTEM, "Loading scene: %s\n", argv[1]);
        ye_load_scene_deferred(argv[1]);
        return;
    }
}