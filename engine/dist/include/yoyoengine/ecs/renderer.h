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

#ifndef YE_RENDERER_H
#define YE_RENDERER_H

#include <yoyoengine/yoyoengine.h>

/*
    Enum to store different unique types of renderers.
    This is how we identify steps needed to render different types of entities.

    Ex: animation renderer knows it needs to increment frames, text renderer knows how to reconstruct text, etc
*/
enum ye_component_renderer_type {
    YE_RENDERER_TYPE_TEXT,
    YE_RENDERER_TYPE_TEXT_OUTLINED,
    YE_RENDERER_TYPE_IMAGE,
    YE_RENDERER_TYPE_ANIMATION
};

struct ye_component_renderer {
    bool active;    // controls whether system will act upon this component

    SDL_Texture *texture;   // texture to render

    // bool texture_cached;   // whether the texture is cached or not

    enum ye_component_renderer_type type;   // denotes which renderer is needed for this entity

    int alpha;  // alpha of texture

    bool flipped_x;
    bool flipped_y;

    union renderer_impl{ // hold the data for the specific renderer type
        struct ye_component_renderer_text *text;
        struct ye_component_renderer_text_outlined *text_outlined;
        struct ye_component_renderer_image *image;
        struct ye_component_renderer_animation *animation;
    } renderer_impl;
};

struct ye_component_renderer_image {
    char *src;  // path to image
};

struct ye_component_renderer_text {
    char *text;         // text to render
    TTF_Font *font;     // font to use
    SDL_Color *color;    // color of text
};

struct ye_component_renderer_text_outlined {
    char *text;                 // text to render
    int outline_size;           // size of text outline
    TTF_Font *font;             // font to use
    SDL_Color *color;           // color of text
    SDL_Color *outline_color;   // color of text outline
};

struct ye_component_renderer_animation {
    char *animation_path;       // path to animation folder
    char *image_format;         // format of image files in animation

    size_t frame_count;         // number of frames in animation

    int frame_delay;            // delay between frames in ms
    int last_updated;           // SDL_GetTicks() last frame advance
    int loops;                  // number of loops, -1 for infinite
    int current_frame_index;    // current frame index

    SDL_Texture** frames;       // array of textures for each frame

    bool paused;
};

/*
    DO NOT USE THIS DIRECTLY UNLESS YOU KNOW WHAT YOURE DOING

    YOU MUST PASS A VOID POINTER TO A STRUCT OF MATCHING TYPE
*/
void ye_add_renderer_component(struct ye_entity *entity, enum ye_component_renderer_type type, void *data);

void ye_temp_add_image_renderer_component(struct ye_entity *entity, char *src);

void ye_temp_add_text_renderer_component(struct ye_entity *entity, char *text, TTF_Font *font, SDL_Color *color);

void ye_temp_add_text_outlined_renderer_component(struct ye_entity *entity, char *text, TTF_Font *font, SDL_Color *color, SDL_Color *outline_color, int outline_size);

void ye_temp_add_animation_renderer_component(struct ye_entity *entity, char *path, char *format, size_t count, int frame_delay, int loops);

void ye_remove_renderer_component(struct ye_entity *entity);

void ye_system_renderer(SDL_Renderer *renderer);

#endif