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

#include <SDL3_mixer/SDL_mixer.h>

#include <uthash/uthash.h>

// counter for audio chunks
extern int totalChunks;

struct ye_mixer_cache_item {
    char *handle;           // the handle of the resource
    MIX_Audio *audio;       // the audio of the resource
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
 * @return MIX_Audio*
 */
YE_API MIX_Audio *ye_audio(const char *handle);

/**
 * @brief Get the global MIX_Mixer device (needed by yep.c for loading audio)
 */
YE_API MIX_Mixer *ye_get_mixer();

/*
    AUDIO SYSTEM
*/

YE_API void ye_init_audio();
YE_API void ye_shutdown_audio();

YE_API MIX_Track *ye_play_sound(const char *handle, int loops, float volume_scale);
YE_API void ye_play_music(const char *handle, int loops, float volume_scale);

/**
 * @brief Set the volume for the entire audio system
 * 
 * @param volume A float from 0.0 to 1.0
 * 
 * @note This will update the global engine volume setting itself
 */
YE_API void ye_set_volume(float volume);

/**
 * @brief Get the number of allocated audio channels
 * 
 * @return The total number of channels allocated
 */
YE_API int ye_get_audio_allocated_channels();

/**
 * @brief Get the number of busy (playing) audio channels
 * 
 * @return The number of channels currently playing audio
 */
YE_API int ye_get_audio_busy_channels();

/**
 * @brief Get the number of items in the mixer cache
 * 
 * @return The count of cached audio chunks
 */
YE_API int ye_get_mixer_cache_count();

YE_API void _ye_audio_decrement_busy();

/**
 * @brief Queue a track for deferred destruction from the main thread.
 *        Use this instead of MIX_DestroyTrack when inside a stopped callback
 *        (which fires from the audio thread).
 */
YE_API void _ye_queue_track_destroy(MIX_Track *track);

/**
 * @brief Destroy all tracks that were queued for deferred destruction from audio thread callbacks.
 *        Must be called from the main thread once per frame (done automatically by ye_system_audiosource).
 */
YE_API void ye_flush_pending_track_destroys();

#endif