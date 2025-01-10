/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file audio.h
 * @brief The engine API for handling audio
 */

#ifndef YE_AUDIO_H
#define YE_AUDIO_H

#include <yoyoengine/export.h>

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

YE_API void ye_init_mixer_cache();
YE_API void ye_shutdown_mixer_cache();
YE_API void ye_purge_mixer_cache();
YE_API void _ye_mixer_engine_cache(char *handle);

/**
 * @brief Load an audio chunk by handle into the cache and return a pointer to it
 * 
 * @param handle 
 * @return Mix_Chunk* 
 */
YE_API Mix_Chunk *ye_audio(const char *handle);

/*
    AUDIO SYSTEM
*/

YE_API void ye_init_audio();
YE_API void ye_shutdown_audio();

YE_API int ye_play_sound(const char *handle, int loops, float volume_scale);
YE_API void ye_play_music(const char *handle, int loops, float volume_scale);

/**
 * @brief Set the volume for the entire audio system
 * 
 * @param volume A float from 0.0 to 1.0
 * 
 * @note This will update the global engine volume setting itself
 */
YE_API void ye_set_volume(float volume);

#endif