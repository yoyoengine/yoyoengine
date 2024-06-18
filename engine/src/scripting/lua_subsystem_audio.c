/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

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