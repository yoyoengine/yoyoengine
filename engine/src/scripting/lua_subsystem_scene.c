/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stdbool.h>

#include <lua.h>

#include <yoyoengine/scene.h>
#include <yoyoengine/lua_api.h>
#include <yoyoengine/logging.h>

int ye_lua_load_scene(lua_State *L) {
    const char *scene_name = lua_tostring(L, 1);

    // defer to next frame to avoid breaking the lua stack
    ye_load_scene_deferred(scene_name);

    return 0;
}

int ye_lua_scene_register(lua_State *L) {
    lua_register(L, "ye_load_scene", ye_lua_load_scene);

    return 0;
}