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

#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>

#include <yoyoengine/logging.h>

// used to align things inside bounds
typedef enum {
    ALIGN_TOP_LEFT, ALIGN_TOP_CENTER, ALIGN_TOP_RIGHT,
    ALIGN_MID_LEFT, ALIGN_MID_CENTER, ALIGN_MID_RIGHT,
    ALIGN_BOT_LEFT, ALIGN_BOT_CENTER, ALIGN_BOT_RIGHT,
    ALIGN_STRETCH // for cases where we dgaf about alignment and just want to stretch anything to bounds
} Alignment;

////////////////////////////////////////////////////// delete the above

/*
    Alignment enum

    Used to describe where an entity is aligned within its bounds.
*/
enum ye_alignment {
    YE_ALIGN_TOP_LEFT,  YE_ALIGN_TOP_CENTER,    YE_ALIGN_TOP_RIGHT,
    YE_ALIGN_MID_LEFT,  YE_ALIGN_MID_CENTER,    YE_ALIGN_MID_RIGHT,
    YE_ALIGN_BOT_LEFT,  YE_ALIGN_BOT_CENTER,    YE_ALIGN_BOT_RIGHT,
    YE_ALIGN_STRETCH
};

/*
    Slow (probably) function that jumps through a bunch of hoops aligning struct rectf's
    It has to convert them to SDL_Rect's and back because the SDL_Rect's are used for rendering
    but could probably be refactored because its just floating point math anyways

    TODO: SDL_Point should be floating too
*/

struct ye_rectf; // this forward declaration ignores a bunch of compiler warnings, and i have no clue why
void ye_auto_fit_bounds(struct ye_rectf* bounds_f, struct ye_rectf* obj_f, enum ye_alignment alignment, SDL_Point* center);

SDL_Rect ye_get_real_texture_size_rect(SDL_Texture *pTexture);

/*
    Floating point rectangle
*/
struct ye_rectf {
    float x, y, w, h;
};

SDL_Rect ye_convert_rectf_rect(struct ye_rectf rect);

struct ye_rectf ye_convert_rect_rectf(SDL_Rect rect);

enum ye_component_type {
    YE_COMPONENT_TRANSFORM,
    YE_COMPONENT_RENDERER,
    YE_COMPONENT_PHYSICS,
    YE_COMPONENT_COLLIDER,
    YE_COMPONENT_SCRIPT,
    YE_COMPONENT_AUDIOSOURCE,
    YE_COMPONENT_CAMERA,
    YE_COMPONENT_TAG
};

#endif