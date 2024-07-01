/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <string.h>

#include <SDL_mixer.h>

#include <yoyoengine/audio.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/ecs/audiosource.h>

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
        newsrc->channel = -10; // ye_play_sound(handle, loops, volume);
        newsrc->playing = true;
    } else {
        newsrc->channel = -10;
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
    // ye_logf(info, "    channel: %d\n", newsrc->channel);
    // ye_logf(info, "    playing: %d\n", newsrc->playing);
}

void ye_remove_audiosource_component(struct ye_entity *entity){
    /*
        Remove an audiosource component from the entity
    */
    free(entity->audiosource->handle);
    free(entity->audiosource);

    // remove the entity from the audiosource list
    ye_entity_list_remove(&audiosource_list_head, entity);

    // mixer cache takes care of removing chunk as needed
}

/*
    TODO:
    this math could be optimized and made better
*/
void ye_system_audiosource(){
    /*
        We are considering the center of the active camera to be the audio listener
    */
    struct ye_rectf camera = ye_get_position(YE_STATE.engine.target_camera, YE_COMPONENT_CAMERA);
    float listener_x, listener_y;
    listener_x = camera.x + (camera.w / 2);
    listener_y = camera.y + (camera.h / 2);

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

        if(src->active){
            if(src->simulated){
                // sanity check to make sure the audiosource takes up space
                if(src->range.w > 0 && src->range.h >= 0){
                    // get the position of the audiosource
                    struct ye_rectf pos = ye_get_position(entity, YE_COMPONENT_AUDIOSOURCE);

                    // find the center float of the audio source
                    float src_center_x = pos.x + (pos.w / 2);
                    float src_center_y = pos.y + (pos.h / 2);

                    // find the distance between src center and listener
                    float distance = ye_distance(listener_x, listener_y, src_center_x, src_center_y);

                    // find the angle between src center and listener
                    float angle = ye_angle(listener_x, listener_y, src_center_x, src_center_y);

                    // scale the angle to make sure due north is 0 degrees
                    angle -= 270;
                    if(angle < 0){
                        angle += 360;
                    }

                    // SDL_Mixer takes in a uint8_t for the distance, so we need to scale the distance to 0-255

                    int distance_from_center_scaled = (int)(distance / (src->range.w / 2) * 255);

                    if(distance_from_center_scaled > 255){
                        // mute the channel
                        Mix_Volume(src->channel, 0);
                    }
                    else{
                        if(src->playing && src->channel == -10){
                            src->channel = ye_play_sound(src->handle, src->loops, src->volume);
                        }

                        // adjust the volume of the channel
                        Mix_Volume(src->channel, (int)(YE_STATE.engine.volume * src->volume));

                        // set the position of the channel
                        Mix_SetPosition(src->channel, (Sint16)angle, (Uint8)distance_from_center_scaled);
                    }
                }
            }
            else{
                // global sound effect (not simulated)
                if(src->playing && src->channel == -10){
                    src->channel = ye_play_sound(src->handle, src->loops, src->volume);
                }
                Mix_Volume(src->channel, (int)(YE_STATE.engine.volume * src->volume));
                // remove any spatial mix
                Mix_SetPosition(src->channel, 0, 0);
            }
        }
    }
}

/*
    Fired from audio.c when a channel finishes
*/
void ye_audiosource_channel_finished(int channel){
    struct ye_entity_node *node;
    for(node = audiosource_list_head; node != NULL; node = node->next){
        // get the entity
        struct ye_entity *entity = node->entity;

        // get the audiosource component
        struct ye_component_audiosource *src = entity->audiosource;

        if(src->channel == channel){
            // if the audiosource is set to loop, we will play it again
            if(src->loops == -1){
                // infinite looping
                src->channel = ye_play_sound(src->handle, src->loops, src->volume);
                src->playing = true;
            } else if(src->loops > 0){
                // finite looping
                src->loops--;
                src->playing = true;
                src->channel = ye_play_sound(src->handle, src->loops, src->volume);
            } else {
                // no looping
                src->playing = false;
            }
        }
    }
}
