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

#ifndef AUDIO_H
#define AUDIO_H

// counter for audio chunks
extern int totalChunks;

// initialize audio components for engine
void initAudio();

// play a sound by its filename path and specify number of loops (-1 for looping)
void playSound(const char *pFilename, int channel, int loops);

// set a specific (or all channels if passed -1) volume level 0-128 
void setVolume(int channel, int volume);

// shut down all audio systems and free all memory assosciated
void shutdownAudio();

#endif