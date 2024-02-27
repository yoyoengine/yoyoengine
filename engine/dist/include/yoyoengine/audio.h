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
 * @file audio.h
 * @brief The engine API for handling audio
 */

#ifndef YE_AUDIO_H
#define YE_AUDIO_H

#include <SDL_mixer.h>

#include <uthash/uthash.h>

// counter for audio chunks
extern int totalChunks;

struct ye_mixer_cache_item {
    char *handle;           // the handle of the resource
    Mix_Chunk *chunk;       // the chunk of the resource
    UT_hash_handle hh;      // the hash handle
};

/*
    MIXER CACHE
*/

void ye_init_mixer_cache();
void ye_shutdown_mixer_cache();
void ye_purge_mixer_cache();
void _ye_mixer_engine_cache(char *handle);

/**
 * @brief Load an audio chunk by handle into the cache and return a pointer to it
 * 
 * @param handle 
 * @return Mix_Chunk* 
 */
Mix_Chunk *ye_audio(const char *handle);

/*
    AUDIO SYSTEM
*/

void ye_init_audio();
void ye_shutdown_audio();

int ye_play_sound(const char *handle, int loops, float volume_scale);
void ye_play_music(const char *handle, int loops, float volume_scale);

/**
 * @brief Set the volume for the entire audio system
 * 
 * @param volume A float from 0.0 to 1.0
 * 
 * @note This will update the global engine volume setting itself
 */
void ye_set_volume(float volume);

#endif