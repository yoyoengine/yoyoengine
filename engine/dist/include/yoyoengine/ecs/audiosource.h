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
 * @file audiosource.h
 * @brief NOT YET DOCUMENTED OR IMPLEMENTED
 */

#ifndef YE_AUDIOSOURCE_H
#define YE_AUDIOSOURCE_H

#include <yoyoengine/yoyoengine.h>

/*
    This spatialized audio relies on:
    - relative positioning options for components
    which relies on:
    - figuring out rect vs bounds

    also:
    - circular areas of effect (optional)?
*/

/*
    For audio sources, we dont really care about some of the traditional priorities for a complete audio api, these are more about very basic
    sounds playing in a scene. they have a position, a volume, and a file name source. Their channel is automatically assigned by the engine. Active means the component is even enabled,
    there is a seperate bool for "playing" which can be used to unload the sound if it is not playing and we are outside of earshot
*/

struct ye_component_audiosource {
    bool active;
    bool playing;
    char *filename;
    int loops;
    int volume;

    // struct ye_position pos; TODO
    
    // reserved for engine tracking and manipulation:

    int channel;

    // honestly why not just load the sound here, we would have to do it anyway, possibly multiple times if we are registering and de registering
};

/*
    Constructs the audio source component for the given entity. The filename is the path to the audio file, and loops is the number of times
*/
void ye_add_audiosource_component(struct ye_entity *entity, const char *pFilename, int loops);

void ye_play_audiosource(struct ye_entity *entity);

void ye_pause_audiosource(struct ye_entity *entity);

void ye_remove_audiosource_component(struct ye_entity *entity);

#endif