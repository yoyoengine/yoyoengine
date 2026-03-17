/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <SDL3_mixer/SDL_mixer.h>
#include <uthash/uthash.h>

#include <yoyoengine/yep.h>
#include <yoyoengine/audio.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/logging.h>
#include <yoyoengine/ecs/audiosource.h>

int totalChunks = 0;

static MIX_Mixer *mixer = NULL;
static MIX_Audio *music_audio = NULL;
static MIX_Track *music_track = NULL;
static int audio_mix_busy_channels = 0;

/*
    ==========================================
                MIXER CACHE IMPL
    ==========================================
*/

struct ye_mixer_cache_item * mix_cache_table = NULL;

void ye_init_mixer_cache()
{
    // initialize the mixer cache
    mix_cache_table = NULL;
    ye_logf(info, "Initialized mixer cache.\n");
}

void ye_shutdown_mixer_cache()
{
    ye_purge_mixer_cache();
    ye_logf(info, "Shut down mixer cache.\n");
}

// free all chunks
void ye_purge_mixer_cache(){
    // iterate through the cache
    struct ye_mixer_cache_item *item, *tmp;
    HASH_ITER(hh, mix_cache_table, item, tmp) {
        // remove the item from the cache
        HASH_DEL(mix_cache_table, item);

        // free the audio
        MIX_DestroyAudio(item->audio);

        // free the item
        free(item->handle);
        free(item);
    }

    // set the cache to null
    mix_cache_table = NULL;
}

/*
    This is intended to cache a brand NEW item, and that we've already checked for duplicates
*/
void ye_mixer_cache(const char *handle)
{
    // create struct to cache
    struct ye_mixer_cache_item *item = malloc(sizeof(struct ye_mixer_cache_item));

    // set the handle
    item->handle = strdup(handle);

    // if in editor mode, retrieve from disk, if runtime load from pack
    if(YE_STATE.editor.editor_mode){
        // load from disk
        item->audio = MIX_LoadAudio(mixer, ye_path_resources(handle), true);

    } else {
        // load from pack
        item->audio = yep_resource_audio(handle);
    }

    // check if the audio is null
    if(item->audio == NULL){
        ye_logf(error, "Failed to load audio chunk %s.\n", handle);
        free(item->handle);
        free(item);
        return;
    }

    // add the item to the cache
    HASH_ADD_KEYPTR(hh, mix_cache_table, item->handle, strlen(item->handle), item);
}

/*
    Cache a mixer chunk from engine data
*/
void _ye_mixer_engine_cache(char *handle)
{
    // create struct to cache
    struct ye_mixer_cache_item *item = malloc(sizeof(struct ye_mixer_cache_item));

    // set the handle
    item->handle = strdup(handle);

    // if in editor mode, retrieve from disk, if runtime load from pack
    if(YE_STATE.editor.editor_mode){
        // load from disk
        item->audio = MIX_LoadAudio(mixer, ye_get_engine_resource_static(handle), true);

    } else {
        // load from pack
        item->audio = yep_engine_resource_audio(handle);
    }

    // check if the audio is null
    if(item->audio == NULL){
        ye_logf(error, "Failed to load engine audio chunk %s.\n", handle);
        free(item->handle);
        free(item);
        return;
    }

    // add the item to the cache
    HASH_ADD_KEYPTR(hh, mix_cache_table, item->handle, strlen(item->handle), item);
}

/*
    Api to return a mix audio from a handle, and load it if not existant
*/
MIX_Audio *ye_audio(const char *handle){
    // check if the cache has an existing audio by this handle
    struct ye_mixer_cache_item *item = NULL;
    HASH_FIND_STR(mix_cache_table, handle, item);

    // if the item is null, we need to cache it
    if(item == NULL){
        // cache the item
        ye_mixer_cache(handle);

        // retrieve the item
        HASH_FIND_STR(mix_cache_table, handle, item);
    }

    if(item == NULL) return NULL;
    return item->audio;
}

/*
    ==========================================
*/




MIX_Mixer *ye_get_mixer(){
    return mixer;
}

void _ye_audio_decrement_busy(){
    if(audio_mix_busy_channels > 0)
        audio_mix_busy_channels--;
    totalChunks = audio_mix_busy_channels;
}

/*
    ==========================================
            AUDIO PLAYBACK INTERFACE
    ==========================================
*/

void ye_init_audio(){
    audio_mix_busy_channels = 0;

    if(!MIX_Init()){
        ye_logf(error, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", SDL_GetError());
        exit(1);
    }

    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if(!mixer){
        ye_logf(error, "SDL_mixer could not create mixer device! SDL_mixer Error: %s\n", SDL_GetError());
        exit(1);
    }
    ye_logf(info, "Initialized SDL_mixer.\n");

    ye_init_mixer_cache();

    MIX_SetMixerGain(mixer, (float)YE_STATE.engine.volume / 128.0f);

    ye_logf(info, "Initialized audio.\n");
}

/*
    TODO: better way to do this? we have to make sure we dont
    free chunks and let audiosource system tentatively reschedule
    them without knowledge that we are done with those sources
*/
static bool mid_audio_shutdown = false;

// stop playing and clear mixer cache, shutdown mixer
void ye_shutdown_audio(){
    mid_audio_shutdown = true;

    MIX_StopAllTracks(mixer, 0);

    if(music_track != NULL){
        MIX_DestroyTrack(music_track);
        music_track = NULL;
    }
    if(music_audio != NULL){
        MIX_DestroyAudio(music_audio);
        music_audio = NULL;
    }

    ye_shutdown_mixer_cache();

    MIX_DestroyMixer(mixer);
    mixer = NULL;

    audio_mix_busy_channels = 0;

    MIX_Quit();
    ye_logf(info, "Shut down audio.\n");

    mid_audio_shutdown = false;
}

/*
    Stopped callback for the music track.
    When music finishes naturally, clean up the track object.
*/
static void ye_music_track_stopped(void *userdata, MIX_Track *track){
    (void)userdata;
    if(!mid_audio_shutdown){
        music_track = NULL;
        MIX_DestroyTrack(track);
    }
}

/*
    Default fire-and-forget stopped callback for sounds not owned by an audiosource.
    Decrements the busy counter and destroys the track.
*/
static void ye_default_track_stopped(void *userdata, MIX_Track *track){
    (void)userdata;
    if(!mid_audio_shutdown){
        _ye_audio_decrement_busy();
        MIX_DestroyTrack(track);
    }
}

/*
    Play a sound by its handle.
    retrieves from cache, creates new track for it
*/
MIX_Track *ye_play_sound(const char *handle, int loops, float volume_scale){
    MIX_Audio *audio = ye_audio(handle);

    if(audio == NULL){
        ye_logf(error, "Failed to play audio chunk %s.\n", handle);
        return NULL;
    }

    MIX_Track *track = MIX_CreateTrack(mixer);
    if(track == NULL){
        ye_logf(error, "Failed to create audio track for %s.\n", handle);
        return NULL;
    }

    MIX_SetTrackAudio(track, audio);
    MIX_SetTrackLoops(track, loops);
    MIX_SetTrackGain(track, (float)YE_STATE.engine.volume / 128.0f * volume_scale);

    // Default fire-and-forget cleanup. Audiosource may override this after calling ye_play_sound.
    MIX_SetTrackStoppedCallback(track, ye_default_track_stopped, NULL);

    MIX_PlayTrack(track, 0);

    audio_mix_busy_channels++;
    totalChunks = audio_mix_busy_channels;

    return track;
}

void ye_play_music(const char *handle, int loops, float volume_scale){
    if(music_track != NULL){
        MIX_StopTrack(music_track, 0);
        MIX_DestroyTrack(music_track);
        music_track = NULL;
    }
    if(music_audio != NULL){
        MIX_DestroyAudio(music_audio);
        music_audio = NULL;
    }

    if(YE_STATE.editor.editor_mode){
        music_audio = MIX_LoadAudio(mixer, ye_path_resources(handle), true);
    } else {
        music_audio = yep_resource_music(handle);
    }

    if(music_audio == NULL){
        ye_logf(error, "Failed to play music %s.\n", handle);
        return;
    }

    music_track = MIX_CreateTrack(mixer);
    if(music_track == NULL){
        ye_logf(error, "Failed to create music track.\n");
        MIX_DestroyAudio(music_audio);
        music_audio = NULL;
        return;
    }

    MIX_SetTrackAudio(music_track, music_audio);
    MIX_SetTrackLoops(music_track, loops);
    MIX_SetTrackGain(music_track, (float)YE_STATE.engine.volume / 128.0f * volume_scale);
    MIX_PlayTrack(music_track, 0);
    MIX_SetTrackStoppedCallback(music_track, ye_music_track_stopped, NULL);
}

/*
    ==========================================
*/

void ye_set_volume(float volume){
    YE_STATE.engine.volume = (int)(128 * volume);
    MIX_SetMixerGain(mixer, volume);
    ye_logf(debug, "Set audio volume to %f.\n", volume);
}

int ye_get_audio_allocated_channels(){
    return audio_mix_busy_channels;  // tracks are created on demand; allocated == busy
}

int ye_get_audio_busy_channels(){
    return audio_mix_busy_channels;
}

int ye_get_mixer_cache_count(){
    unsigned int count = HASH_COUNT(mix_cache_table);
    return (int)count;
}
