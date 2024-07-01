/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stdbool.h>

#include <lua.h>

#include <yoyoengine/audio.h>
#include <yoyoengine/lua_api.h>
#include <yoyoengine/logging.h>

/*
    AUDIO

    notes:
    - this is not a super complete api, i feel like its better to build it out over time
    as needed rather than spam features up front.
    - Could use: change volume scale of a channel or music specifically, terminate channel or music,
    info on number of channels playing, etc.
*/

int ye_lua_audio_play_sound(lua_State *L) {
    const char *handle = lua_tostring(L, 1);
    int loops = lua_tointeger(L, 2);
    float volume_scale = lua_tonumber(L, 3);

    ye_play_sound(handle, loops, volume_scale);

    // ye_play_sound returns the channel assigned, but im going to drop it for now.

    return 0;
}

int ye_lua_audio_play_music(lua_State *L) {
    const char *handle = lua_tostring(L, 1);
    int loops = lua_tointeger(L, 2);
    float volume_scale = lua_tonumber(L, 3);

    ye_play_music(handle, loops, volume_scale);

    return 0;
}

int ye_lua_audio_set_volume(lua_State *L) {
    float volume = lua_tonumber(L, 1);
    ye_set_volume(volume);

    return 0;
}

int ye_lua_audio_register(lua_State *L) {
    lua_register(L, "ye_audio_play_sound", ye_lua_audio_play_sound);
    lua_register(L, "ye_audio_play_music", ye_lua_audio_play_music);
    lua_register(L, "ye_audio_set_volume", ye_lua_audio_set_volume);

    return 0;
}