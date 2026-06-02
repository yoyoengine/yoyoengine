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
        DEFERRED TRACK DESTRUCTION
    ==========================================
    Stopped callbacks fire from the audio thread (inside SDL_GetAudioStreamDataAdjustGain),
    which holds output_stream's lock. Calling MIX_DestroyTrack there frees the stream
    and its queue out from under the caller, causing a use-after-free SIGSEGV.
    Instead, we queue tracks here and destroy them from the main thread.
*/

#define YE_MAX_PENDING_TRACK_DESTROYS 128
static MIX_Track *pending_track_destroys[YE_MAX_PENDING_TRACK_DESTROYS];
static int pending_track_destroy_count = 0;
static SDL_Mutex *pending_track_destroy_lock = NULL;

void _ye_queue_track_destroy(MIX_Track *track)
{
    SDL_LockMutex(pending_track_destroy_lock);
    if(pending_track_destroy_count < YE_MAX_PENDING_TRACK_DESTROYS){
        pending_track_destroys[pending_track_destroy_count++] = track;
    } else {
        ye_logf(warning, "Pending track destroy queue full! Track leak possible.\n");
    }
    SDL_UnlockMutex(pending_track_destroy_lock);
}

void ye_flush_pending_track_destroys()
{
    /*
        Snapshot the queue under the lock, then release it BEFORE calling
        MIX_DestroyTrack. MIX_DestroyTrack synchronizes with the audio thread
        (it tears down the track's output stream). Meanwhile the audio thread,
        inside a stopped callback, holds that same stream lock and calls
        _ye_queue_track_destroy(), which blocks on pending_track_destroy_lock.

        Holding pending_track_destroy_lock across MIX_DestroyTrack therefore
        inverts the lock order against the audio thread and deadlocks the game
        (hard freeze). Doing the destroys outside the lock breaks the cycle.
    */
    MIX_Track *to_destroy[YE_MAX_PENDING_TRACK_DESTROYS];
    int count = 0;

    SDL_LockMutex(pending_track_destroy_lock);
    count = pending_track_destroy_count;
    for(int i = 0; i < count; i++){
        to_destroy[i] = pending_track_destroys[i];
    }
    pending_track_destroy_count = 0;
    SDL_UnlockMutex(pending_track_destroy_lock);

    for(int i = 0; i < count; i++){
        MIX_DestroyTrack(to_destroy[i]);
    }
}

/*
    ==========================================
*/

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

    pending_track_destroy_count = 0;
    pending_track_destroy_lock = SDL_CreateMutex();

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

    // Flush any tracks that were queued for deferred destruction before shutdown began
    ye_flush_pending_track_destroys();

    SDL_DestroyMutex(pending_track_destroy_lock);
    pending_track_destroy_lock = NULL;

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
    NOTE: This fires from the audio thread. Do NOT call MIX_DestroyTrack here —
    we are inside SDL_GetAudioStreamDataAdjustGain which still holds output_stream's
    lock and queue reference. Queue for deferred destruction on the main thread.
*/
static void ye_music_track_stopped(void *userdata, MIX_Track *track){
    (void)userdata;
    if(!mid_audio_shutdown){
        music_track = NULL;
        _ye_queue_track_destroy(track);
    }
}

/*
    Default fire-and-forget stopped callback for sounds not owned by an audiosource.
    NOTE: This fires from the audio thread. Do NOT call MIX_DestroyTrack here —
    queue for deferred destruction on the main thread.
*/
static void ye_default_track_stopped(void *userdata, MIX_Track *track){
    (void)userdata;
    if(!mid_audio_shutdown){
        _ye_audio_decrement_busy();
        _ye_queue_track_destroy(track);
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
    MIX_SetTrackGain(track, (float)YE_STATE.engine.volume / 128.0f * volume_scale);

    // Default fire-and-forget cleanup. Audiosource may override this after calling ye_play_sound.
    MIX_SetTrackStoppedCallback(track, ye_default_track_stopped, NULL);

    // MIX_SetTrackLoops has no effect on stopped tracks; loops must be passed via properties
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
    MIX_PlayTrack(track, props);
    SDL_DestroyProperties(props);

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
    MIX_SetTrackGain(music_track, (float)YE_STATE.engine.volume / 128.0f * volume_scale);
    MIX_SetTrackStoppedCallback(music_track, ye_music_track_stopped, NULL);

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
    MIX_PlayTrack(music_track, props);
    SDL_DestroyProperties(props);
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
