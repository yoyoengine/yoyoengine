/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <string.h>
#include <stdio.h>

#include <yoyoengine/logging.h>
#include <yoyoengine/console.h>
#include <yoyoengine/commands.h>

#include <yoyoengine/ui/overlays.h>

void _overlay_usage() {
    ye_logf(_YE_RESERVED_LL_SYSTEM, "Usage:\n");
    ye_logf(_YE_RESERVED_LL_SYSTEM, "       overlay list : list all registered overlays\n");
    ye_logf(_YE_RESERVED_LL_SYSTEM, "       overlay show [value] : show an overlay\n");
    ye_logf(_YE_RESERVED_LL_SYSTEM, "       overlay hide [value] : hide an overlay\n");
}

bool _search_and_set(const char *name, bool value) {
    for(size_t i = 0; i < overlays->size; i++) {
        struct ye_overlay *overlay = (struct ye_overlay*)ye_vector_get(overlays, i);
        if(strcmp(overlay->name, name) == 0) {
            overlay->active = value;
            return true;
        }
    }
    return false;
}

void ye_cmd_overlay(int argc, const char **argv) {
    printf("argc: %d\n", argc);
    if(argc < 1) {
        _overlay_usage();
        return;
    }

    if(argv && argv[0] && strcmp(argv[0], "list") == 0) {
        if(argc > 1) {
            ye_logf(_YE_RESERVED_LL_SYSTEM, "warning: too many args (%d). Ignoring argc>1.\n",argc);
        }

        ye_logf(_YE_RESERVED_LL_SYSTEM, "Available overlays:\n");
        YE_VECTOR_FOR_EACH(overlays, struct ye_overlay, overlay) {
            ye_logf(_YE_RESERVED_LL_SYSTEM, "    %s\n", overlay.name);
        }
        return;
    }


    if(strcmp(argv[0], "show") == 0) {
        if(argc > 2) {
            ye_logf(_YE_RESERVED_LL_SYSTEM, "warning: too many args (%d). Ignoring argc>2.\n",argc);
        }

        if(!_search_and_set(argv[1], true)) {
            ye_logf(_YE_RESERVED_LL_SYSTEM, "error: could not show overlay: \"%s\". Does it exist?\n", argv[1]);
        }
        else {
            ye_logf(_YE_RESERVED_LL_SYSTEM, "Set overlay: \"%s\" to shown\n", argv[1]);
        }
    }
    else if(strcmp(argv[0], "hide") == 0) {
        if(argc > 2) {
            ye_logf(_YE_RESERVED_LL_SYSTEM, "warning: too many args (%d). Ignoring argc>2.\n",argc);
        }

        if(!_search_and_set(argv[1], false)) {
            ye_logf(_YE_RESERVED_LL_SYSTEM, "error: could not hide overlay: \"%s\". Does it exist?\n", argv[1]);
        }
        else {
            ye_logf(_YE_RESERVED_LL_SYSTEM, "Set overlay: \"%s\" to hidden\n", argv[1]);
        }
    }
    else {
        ye_logf(_YE_RESERVED_LL_SYSTEM, "Invalid command: \"%s\"\n", argv[0]);
        _overlay_usage();
    }
}