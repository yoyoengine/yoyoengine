/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
    @file yoyoengine.h
    @brief The master header for yoyoengine. Includes all other headers.
*/

#ifndef YE_ENGINE_MAIN_H
#define YE_ENGINE_MAIN_H
#define NK_INCLUDE_FIXED_TYPES

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <yoyoengine/export.h>

/*
    Include all our headers in one place.
    This lets the game use
    #include <yoyoengine/yoyoengine.h> assuming they have
    defined the header and lib paths correctly.
*/
#include "version.h"        // version info
#include "engine.h"         // init and core
#include "config.h"         // jansson wrapper for config files
#include "networking.h"
#include "event.h"          // event handling
#include "input.h"
#include "yep.h"            // custom binary format parser/packer
#include "json.h"           // jansson wrapper
#include "graphics.h"
#include "debug_renderer.h"
#include "uthash/uthash.h"
#include "cache.h"
#include "ui.h"
#include "ecs/ecs.h"
#include "ecs/button.h"
#include "ecs/audiosource.h"
#include "ecs/camera.h"
#include "ecs/ecs.h"
#include "ecs/physics.h"
#include "ecs/renderer.h"
#include "ecs/transform.h"
#include "ecs/collider.h"
#include "ecs/tag.h"
#include "ecs/lua_script.h"
#include "utils.h"
#include "timer.h"
#include "audio.h"
#include "logging.h"        // logging
#include "lua_api.h"        // scripting api
#include "scene.h"          // scene manager
#include "tricks.h"         // plugin system

#endif // YE_ENGINE_MAIN_H