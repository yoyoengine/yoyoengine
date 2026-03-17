/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <string.h>

#include <SDL3_mixer/SDL_mixer.h>

#include <yoyoengine/audio.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/ecs/audiosource.h>

/*
    Per-track stopped callback for audiosource components.
    Fires when a track finishes all its loops.
    userdata is the ye_component_audiosource* that owns the track.
    NOTE: This fires from the audio thread (inside SDL_GetAudioStreamDataAdjustGain).
    Do NOT call MIX_DestroyTrack here — the stream's queue is still live in the caller.
    Queue for deferred destruction on the main thread instead.
*/
static void ye_audiosource_track_stopped(void *userdata, MIX_Track *track){
    struct ye_component_audiosource *src = (struct ye_component_audiosource *)userdata;
    src->track = NULL;
    src->playing = false;
    _ye_audio_decrement_busy();
    _ye_queue_track_destroy(track);
}

void ye_add_audiosource_component(struct ye_entity *entity, const char *handle, float volume, bool play_on_awake, int loops, bool simulated, struct ye_rectf range){
    /*
        Add an audiosource component to the entity
    */
    struct ye_component_audiosource *newsrc = malloc(sizeof(struct ye_component_audiosource));

    // alloc the handle
    newsrc->handle = strdup(handle);

    // assign fields
    newsrc->volume = volume;
    newsrc->play_on_awake = play_on_awake;
    newsrc->loops = loops;
    newsrc->active = true;
    newsrc->simulated = simulated;
    newsrc->range = range;
    newsrc->relative = true;

    // add the component to the entity
    entity->audiosource = newsrc;

    // add the entity to the audiosource list
    ye_entity_list_add(&audiosource_list_head, entity);

    if(play_on_awake && !YE_STATE.editor.editor_mode){
        // play the sound
        newsrc->track = NULL; // ye_play_sound(handle, loops, volume);
        newsrc->playing = true;
    } else {
        newsrc->track = NULL;
        newsrc->playing = false;
    }

    // print every piece of info about the audiosource
    // ye_logf(info, "Added audiosource component to entity %s\n", entity->name);
    // ye_logf(info, "    handle: %s\n", handle);
    // ye_logf(info, "    volume: %f\n", volume);
    // ye_logf(info, "    play_on_awake: %d\n", play_on_awake);
    // ye_logf(info, "    loops: %d\n", loops);
    // ye_logf(info, "    active: %d\n", true);
    // ye_logf(info, "    simulated: %d\n", simulated);
    // ye_logf(info, "    range: %f, %f, %f, %f\n", range.x, range.y, range.w, range.h);
    // ye_logf(info, "    relative: %d\n", true);
    // ye_logf(info, "    track: %p\n", (void*)newsrc->track);
    // ye_logf(info, "    playing: %d\n", newsrc->playing);
}

void ye_remove_audiosource_component(struct ye_entity *entity){
    struct ye_component_audiosource *src = entity->audiosource;

    // Stop any active track before freeing to prevent the callback from
    // firing on freed memory
    if(src->track != NULL){
        MIX_SetTrackStoppedCallback(src->track, NULL, NULL);
        MIX_StopTrack(src->track, 0);
        MIX_DestroyTrack(src->track);
        src->track = NULL;
        _ye_audio_decrement_busy();
    }

    free(src->handle);
    free(src);
    entity->audiosource = NULL;

    ye_entity_list_remove(&audiosource_list_head, entity);
}

void ye_play_audiosource(struct ye_entity *entity){
    if(!entity || !entity->audiosource) return;
    entity->audiosource->playing = true;
}

void ye_pause_audiosource(struct ye_entity *entity){
    if(!entity || !entity->audiosource) return;
    struct ye_component_audiosource *src = entity->audiosource;

    if(src->track != NULL){
        // Clear the callback before stopping to avoid it firing on already-handled state
        MIX_SetTrackStoppedCallback(src->track, NULL, NULL);
        MIX_StopTrack(src->track, 0);
        MIX_DestroyTrack(src->track);
        src->track = NULL;
        _ye_audio_decrement_busy();
    }
    src->playing = false;
}

/*
    TODO:
    this math could be optimized and made better
*/
void ye_system_audiosource(){
    // Destroy tracks that were queued for deferred cleanup by audio thread callbacks
    ye_flush_pending_track_destroys();

    /*
        We are considering the center of the active camera to be the audio listener
    */
    struct ye_point_rectf camera = ye_get_position2(YE_STATE.engine.target_camera, YE_COMPONENT_CAMERA);
    struct ye_pointf cam_cent = ye_point_rectf_center(camera);
    
    float listener_x = cam_cent.x;
    float listener_y = cam_cent.y;

    /*
        For every audiosource in the list, check if it is active and if it is spatial.
        Get its position and calculate the distance and angle from the listener.
        The component has a range, which the outside of is considered to be 0 volume and the center of is considered to be 128 volume.
        We will also scale this against the "volume" set for the component so in reality the center is the max we want it to be.
    */
    struct ye_entity_node *node;
    for(node = audiosource_list_head; node != NULL; node = node->next){
        // get the entity
        struct ye_entity *entity = node->entity;

        // get the audiosource component
        struct ye_component_audiosource *src = entity->audiosource;

        if(!entity || !entity->active || !src || !src->active){
            continue;
        }

        if(src->active){
            if(src->simulated){
                // sanity check to make sure the audiosource takes up space
                if(src->range.w > 0 && src->range.h >= 0){
                    // get the position of the audiosource
                    struct ye_point_rectf pos = ye_get_position2(entity, YE_COMPONENT_AUDIOSOURCE);

                    /*
                        Center, fallof start and max
                    */
                    float cx = pos.verticies[0].x;
                    float cy = pos.verticies[0].y;
                    float min_radius = src->range.h;
                    float max_radius = src->range.w;

                    // find the distance between src center and listener
                    float distance = ye_distance(listener_x, listener_y, cx, cy);

                    /*
                        If we are within falloff ring, play at full volume
                    */
                    if(distance < min_radius){
                        distance = 0;
                    }
                    else {
                        /*
                            Outside the falloff ring we scale the distance between
                            the observer and the beginning of the falloff ring

                            TODO: maybe a better way to do this,
                            stupid hack to make sure the division
                            for distance_from_center_scaled is proper
                        */
                        if(!(min_radius >= max_radius)) // if the rings dont overlap, scale distance
                            distance -= min_radius;
                        else // if rings overlap, erase fallback ring (hack)
                            src->range.h = 0;
                    }

                    // SDL_Mixer takes in a uint8_t for the distance, so we need to scale the distance to 0-255
                    // scale taking into account the falloff ring
                    int distance_from_center_scaled = (int)(distance / ((max_radius) - (min_radius)) * 255);

                    if(distance_from_center_scaled > 255){
                        // outside max range — mute if playing
                        if(src->track != NULL){
                            MIX_SetTrackGain(src->track, 0.0f);
                        }
                    }
                    else{
                        if(src->playing && src->track == NULL){
                            src->track = ye_play_sound(src->handle, src->loops, src->volume);
                            if(src->track != NULL){
                                // Override the default fire-and-forget callback with our own
                                MIX_SetTrackStoppedCallback(src->track, ye_audiosource_track_stopped, src);
                            }
                        }

                        if(src->track != NULL){
                            // Volume: engine volume * component volume * distance falloff
                            float gain = ((float)YE_STATE.engine.volume / 128.0f)
                                       * src->volume
                                       * (1.0f - (float)distance_from_center_scaled / 255.0f);
                            MIX_SetTrackGain(src->track, gain);

                            // Stereo pan based on horizontal offset — MIX_SetTrack3DPosition
                            // includes SDL_mixer's own distance attenuation which conflicts
                            // with our manual gain and uses raw pixel units it can't interpret.
                            float pan = (cx - listener_x) / max_radius;
                            if(pan < -1.0f) pan = -1.0f;
                            if(pan > 1.0f) pan = 1.0f;
                            MIX_StereoGains stereo = {
                                1.0f - (pan > 0.0f ? pan : 0.0f),
                                1.0f + (pan < 0.0f ? pan : 0.0f)
                            };
                            MIX_SetTrackStereo(src->track, &stereo);
                        }
                    }
                }
            }
            else{
                // global sound effect (not simulated)
                if(src->playing && src->track == NULL){
                    src->track = ye_play_sound(src->handle, src->loops, src->volume);
                    if(src->track != NULL){
                        MIX_SetTrackStoppedCallback(src->track, ye_audiosource_track_stopped, src);
                    }
                }
                if(src->track != NULL){
                    MIX_SetTrackGain(src->track, ((float)YE_STATE.engine.volume / 128.0f) * src->volume);
                }
            }
        }
    }
}

