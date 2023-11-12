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

// counter for audio chunks
extern int totalChunks;

/**
 * @brief Initialize audio components for engine.
 */
void ye_audio_init();

/**
 * @brief Play a sound by its filename path and specify number of loops.
 * @param pFilename The path to the audio file.
 * @param channel The audio channel to play the sound on.
 * @param loops The number of times to loop the sound. -1 for infinite looping.
 */
void ye_play_sound(const char *pFilename, int channel, int loops);

/**
 * @brief Set a specific (or all channels if passed -1) volume level 0-128.
 * @param channel The audio channel to set the volume for. -1 for all channels.
 * @param volume The volume level to set. Range is 0-128.
 */
void ye_set_volume(int channel, int volume);

/**
 * @brief Shut down all audio systems and free all memory associated.
 */
void ye_audio_shutdown();

#endif