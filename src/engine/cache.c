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

#include <yoyoengine/yoyoengine.h>
#include <string.h>

/*
    Head nodes for our lists tracking cached resources
*/
struct ye_texture_node * cached_textures_head;
struct ye_font_node * cached_fonts_head;
struct ye_color_node * cached_colors_head;

void ye_init_cache(char *styles_path){
    cached_textures_head = NULL;
    cached_fonts_head = NULL;
    cached_colors_head = NULL;

    if(styles_path == NULL){
        ye_logf(debug,"%s","No styles path provided.");
        return;
    }

    // TODO: implement setup that occurs to load styles path
}

void ye_clear_texture_cache(){
    // free cached textures
    struct ye_texture_node *texture_node, *texture_tmp;
    HASH_ITER(hh, cached_textures_head, texture_node, texture_tmp) {
        HASH_DEL(cached_textures_head, texture_node);
        SDL_DestroyTexture(texture_node->texture);
        free(texture_node->path);
        free(texture_node);
    }
}

void ye_clear_font_cache(){
    // free cached fonts
    struct ye_font_node *font_node, *font_tmp;
    HASH_ITER(hh, cached_fonts_head, font_node, font_tmp) {
        HASH_DEL(cached_fonts_head, font_node);
        TTF_CloseFont(font_node->font);
        free(font_node->name);
        free(font_node);
    }
}

void ye_clear_color_cache(){
    // free cached colors
    struct ye_color_node *color_node, *color_tmp;
    HASH_ITER(hh, cached_colors_head, color_node, color_tmp) {
        HASH_DEL(cached_colors_head, color_node);
        free(color_node->name);
        free(color_node);
    }
}

void ye_shutdown_cache(){
    // free cached textures
    ye_clear_texture_cache();

    // free cached fonts
    ye_clear_font_cache();

    // free cached colors
    ye_clear_color_cache();

    ye_logf(info,"%s","Shut down cache.\n");
}

/*
    PRIMARY API:
    This is the intended interface with the cache system, but assumes you have pre cached fonts and colors.
*/

SDL_Texture * ye_image(char *path){
    // check cache for texture named by path
    struct ye_texture_node *node = cached_textures_head;
    HASH_FIND_STR(cached_textures_head, path, node);
    if(node != NULL){
        // ye_logf(debug,"CACHE HIT: %s\n",path);
        return node->texture;
    }

    // if not found, load texture and add to cache
    // ye_logf(warning,"CACHE MISS: %s\n",path);
    return ye_cache_texture(path);
}

TTF_Font * ye_font(char *name){
    // check cache for font named by name and size
    struct ye_font_node *node;
    for(node = cached_fonts_head; node != NULL; node = node->hh.next) {
        if(strcmp(node->name, name) == 0) {
            // ye_logf(debug,"CACHE HIT: %s\n",name);
            return node->font;
        }
    }

    ye_logf(error,"Font cache miss: %s. Returning default.\n",name);
    return pEngineFont;
}

SDL_Color * ye_color(char *name){
    // check cache for color named by name
    struct ye_color_node *node;
    for(node = cached_colors_head; node != NULL; node = node->hh.next) {
        if(strcmp(node->name, name) == 0) {
            // ye_logf(debug,"CACHE HIT: %s\n",name);
            return &node->color;
        }
    }

    ye_logf(error,"Color cache miss: %s. Returning default.\n",name);
    return pEngineFontColor;
}

/*
    EXTENDED API:
    This is used by the primary API but can also be used directly by the developer.
*/

SDL_Texture * ye_cache_texture(char *path){
    SDL_Texture *texture = ye_create_image_texture(path);

    // cache the texture
    struct ye_texture_node *new_node = malloc(sizeof(struct ye_texture_node));
    new_node->texture = texture;
    new_node->path = malloc(strlen(path) + 1);
    strcpy(new_node->path, path);
    HASH_ADD_KEYPTR(hh, cached_textures_head, new_node->path, strlen(new_node->path), new_node);
    // ye_logf(debug,"Cached texture: %s\n",path);
    return texture;
}

TTF_Font * ye_cache_font(char *name, int size, char *path){
    TTF_Font *font = ye_load_font(path, size);

    // cache the font
    struct ye_font_node *new_node = malloc(sizeof(struct ye_font_node));
    new_node->font = font;
    new_node->name = malloc(strlen(name) + 1);
    strcpy(new_node->name, name);
    new_node->size = size;
    HASH_ADD_KEYPTR(hh, cached_fonts_head, new_node->name, strlen(new_node->name), new_node);
    // ye_logf(debug,"Cached font: %s\n",name);
    return font;
}

SDL_Color * ye_cache_color(char *name, SDL_Color color){
    // cache the color
    struct ye_color_node *new_node = malloc(sizeof(struct ye_color_node));
    new_node->color = color;
    new_node->name = malloc(strlen(name) + 1);
    strcpy(new_node->name, name);
    HASH_ADD_KEYPTR(hh, cached_colors_head, new_node->name, strlen(new_node->name), new_node);
    // ye_logf(debug,"Cached color: %s\n",name);
    return &new_node->color;
}