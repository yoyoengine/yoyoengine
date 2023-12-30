/*
    This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/**
    @file yoyoengine.h
    @brief The master header for yoyoengine. Includes all other headers.
*/

#ifndef YE_ENGINE_MAIN_H
#define YE_ENGINE_MAIN_H
#define NK_INCLUDE_FIXED_TYPES

/*
    Include all our headers in one place.
    This lets the game use
    #include <yoyoengine/yoyoengine.h> assuming they have
    defined the header and lib paths correctly.
*/
#include "engine.h"
#include "json.h"
#include "graphics.h"
#include "uthash.h"
#include "cache.h"
#include "ui.h"
#include "ecs/ecs.h"
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
#include "logging.h"
#include "lua_api.h"
#include "scene.h"

#endif // YE_ENGINE_MAIN_H