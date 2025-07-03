/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/*
    NOTE:
    there is a way to do this without boilerplate by leveraging
    offsetof, but there are not that many engine config fields
    right now so I'm just going to do it this way.
*/

#include <string.h>

#include <yoyoengine/engine.h>
#include <yoyoengine/logging.h>
#include <yoyoengine/commands.h>

void _config_usage() {
    ye_logf(_YE_RESERVED_LL_SYSTEM, "Usage:\n");
    ye_logf(_YE_RESERVED_LL_SYSTEM, "       config list : list all available config variables\n");
    ye_logf(_YE_RESERVED_LL_SYSTEM, "       config get [field] : output the value of a config\n");
    ye_logf(_YE_RESERVED_LL_SYSTEM, "       config set [field] [value] : set the value of a config\n");
}

void _config_get_bool(const char *field, bool *value) {
    ye_logf(_YE_RESERVED_LL_SYSTEM, "%s: %d\n", field, *value);
}

void _config_get_int(const char *field, int *value) {
    ye_logf(_YE_RESERVED_LL_SYSTEM, "%s: %d\n", field, *value);
}

void _config_set_bool(const char *key, bool *field, bool value) {
    *field = value;
    ye_logf(_YE_RESERVED_LL_SYSTEM, "Set %s to %d\n", key, value);
}

void _config_set_int(const char *key, int *field, int value) {
    *field = value;
    ye_logf(_YE_RESERVED_LL_SYSTEM, "Set %s to %d\n", key, value);
}

void ye_cmd_config(int argc, const char **argv) {
    if(argc < 2) {
        if(argv && argv[0] && strcmp(argv[0], "list") == 0) {
            ye_logf(_YE_RESERVED_LL_SYSTEM, "Available config fields:\n");
            ye_logf(_YE_RESERVED_LL_SYSTEM, "    [int]  screen_width\n");
            ye_logf(_YE_RESERVED_LL_SYSTEM, "    [int]  screen_height\n");
            ye_logf(_YE_RESERVED_LL_SYSTEM, "    [int]  window_mode\n");
            ye_logf(_YE_RESERVED_LL_SYSTEM, "    [int]  volume\n");
            ye_logf(_YE_RESERVED_LL_SYSTEM, "    [int]  framecap\n");
            ye_logf(_YE_RESERVED_LL_SYSTEM, "    [int]  sdl_quality_hint\n");
            ye_logf(_YE_RESERVED_LL_SYSTEM, "    [bool] stretch_resolution\n");
            ye_logf(_YE_RESERVED_LL_SYSTEM, "    [bool] paintbounds_visible\n");
            ye_logf(_YE_RESERVED_LL_SYSTEM, "    [bool] wireframe_visible\n");
            ye_logf(_YE_RESERVED_LL_SYSTEM, "    [bool] colliders_visible\n");
            ye_logf(_YE_RESERVED_LL_SYSTEM, "    [bool] display_names\n");
            ye_logf(_YE_RESERVED_LL_SYSTEM, "    [bool] freecam_enabled\n");
            ye_logf(_YE_RESERVED_LL_SYSTEM, "    [bool] audiorange_visible\n");
            ye_logf(_YE_RESERVED_LL_SYSTEM, "    [bool] button_bounds_visible\n");
            return;
        }
        else{
            _config_usage();
            return;
        }
    }


    if(strcmp(argv[0], "get") == 0) {
        if(argc > 2) {
            ye_logf(_YE_RESERVED_LL_SYSTEM, "warning: too many args (%d). Ignoring argc>2.\n",argc);
        }

        if(strcmp(argv[1], "screen_width") == 0)
            _config_get_int("screen_width", &YE_STATE.engine.screen_width);
        
        else if(strcmp(argv[1], "screen_height") == 0)
            _config_get_int("screen_height", &YE_STATE.engine.screen_height);
        
        else if(strcmp(argv[1], "fullscreen") == 0)
            _config_get_bool("fullscreen", &YE_STATE.engine.fullscreen);
        
        else if(strcmp(argv[1], "volume") == 0)
            _config_get_int("volume", &YE_STATE.engine.volume);
        
        else if(strcmp(argv[1], "framecap") == 0)
            _config_get_int("framecap", &YE_STATE.engine.framecap);
        
        else if(strcmp(argv[1], "sdl_quality_hint") == 0)
            _config_get_int("sdl_quality_hint", &YE_STATE.engine.sdl_quality_hint);
        
        else if(strcmp(argv[1], "stretch_resolution") == 0)
            _config_get_bool("stretch_resolution", &YE_STATE.engine.stretch_resolution);
        
        else if(strcmp(argv[1], "paintbounds_visible") == 0)
            _config_get_bool("paintbounds_visible", &YE_STATE.editor.paintbounds_visible);
        
        else if(strcmp(argv[1], "wireframe_visible") == 0)
            _config_get_bool("wireframe_visible", &YE_STATE.editor.wireframe_visible);
        
        else if(strcmp(argv[1], "colliders_visible") == 0)
            _config_get_bool("colliders_visible", &YE_STATE.editor.colliders_visible);
        
        else if(strcmp(argv[1], "display_names") == 0)
            _config_get_bool("display_names", &YE_STATE.editor.display_names);
        
        else if(strcmp(argv[1], "freecam_enabled") == 0)
            _config_get_bool("freecam_enabled", &YE_STATE.editor.freecam_enabled);
        
        else if(strcmp(argv[1], "audiorange_visible") == 0)
            _config_get_bool("audiorange_visible", &YE_STATE.editor.audiorange_visible);
        
        else if(strcmp(argv[1], "button_bounds_visible") == 0)
            _config_get_bool("button_bounds_visible", &YE_STATE.editor.button_bounds_visible);
        
        else
            ye_logf(_YE_RESERVED_LL_SYSTEM, "Error: unknown config field \"%s\"\n", argv[1]);
    }
    else if(strcmp(argv[0], "set") == 0) {
        if(argc < 3) {
            ye_logf(_YE_RESERVED_LL_SYSTEM, "Error: not enough args (%d). Ignoring argc<3.\n",argc);
            _config_usage();
            return;
        }

        if(strcmp(argv[1], "screen_width") == 0)
            _config_set_int("screen_width", &YE_STATE.engine.screen_width, atoi(argv[2]));
        
        else if(strcmp(argv[1], "screen_height") == 0)
            _config_set_int("screen_height", &YE_STATE.engine.screen_height, atoi(argv[2]));
        
        else if(strcmp(argv[1], "fullscreen") == 0)
            _config_set_bool("fullscreen", &YE_STATE.engine.fullscreen, atoi(argv[2]));
        
        else if(strcmp(argv[1], "volume") == 0)
            _config_set_int("volume", &YE_STATE.engine.volume, atoi(argv[2]));
        
        else if(strcmp(argv[1], "framecap") == 0)
            _config_set_int("framecap", &YE_STATE.engine.framecap, atoi(argv[2]));
        
        else if(strcmp(argv[1], "sdl_quality_hint") == 0)
            _config_set_int("sdl_quality_hint", &YE_STATE.engine.sdl_quality_hint, atoi(argv[2]));
        
        else if(strcmp(argv[1], "stretch_resolution") == 0)
            _config_set_bool("stretch_resolution", &YE_STATE.engine.stretch_resolution, atoi(argv[2]));
        
        else if(strcmp(argv[1], "wireframe_visible") == 0)
            _config_set_bool("wireframe_visible", &YE_STATE.editor.wireframe_visible, atoi(argv[2]));

        else if(strcmp(argv[1], "paintbounds_visible") == 0)
            _config_set_bool("paintbounds_visible", &YE_STATE.editor.paintbounds_visible, atoi(argv[2]));
        
        else if(strcmp(argv[1], "colliders_visible") == 0)
            _config_set_bool("colliders_visible", &YE_STATE.editor.colliders_visible, atoi(argv[2]));
        
        else if(strcmp(argv[1], "display_names") == 0)
            _config_set_bool("display_names", &YE_STATE.editor.display_names, atoi(argv[2]));
        
        else if(strcmp(argv[1], "freecam_enabled") == 0)
            _config_set_bool("freecam_enabled", &YE_STATE.editor.freecam_enabled, atoi(argv[2]));
        
        else if(strcmp(argv[1], "audiorange_visible") == 0)
            _config_set_bool("audiorange_visible", &YE_STATE.editor.audiorange_visible, atoi(argv[2]));
        
        else if(strcmp(argv[1], "button_bounds_visible") == 0)
            _config_set_bool("button_bounds_visible", &YE_STATE.editor.button_bounds_visible, atoi(argv[2]));
        
        else
            ye_logf(_YE_RESERVED_LL_SYSTEM, "Error: unknown config field \"%s\"\n", argv[1]);
    }
    else {
        _config_usage();
    }
}