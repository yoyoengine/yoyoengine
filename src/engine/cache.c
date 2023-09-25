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

void ye_shutdown_cache(){
    // free cached textures
    struct ye_texture_node *texture_node, *texture_tmp;
    HASH_ITER(hh, cached_textures_head, texture_node, texture_tmp) {
        HASH_DEL(cached_textures_head, texture_node);
        SDL_DestroyTexture(texture_node->texture);
        free(texture_node->path);
        free(texture_node);
    }

    // free cached fonts
    struct ye_font_node *font_node, *font_tmp;
    HASH_ITER(hh, cached_fonts_head, font_node, font_tmp) {
        HASH_DEL(cached_fonts_head, font_node);
        TTF_CloseFont(font_node->font);
        free(font_node->name);
        free(font_node);
    }

    // free cached colors
    struct ye_color_node *color_node, *color_tmp;
    HASH_ITER(hh, cached_colors_head, color_node, color_tmp) {
        HASH_DEL(cached_colors_head, color_node);
        free(color_node->name);
        free(color_node);
    }

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