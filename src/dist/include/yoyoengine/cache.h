/*
    TODO:
    - Cache size counter could be cool, but not necessary

    Cache System:
    
    The goal of this cache system is to be something that supports the engine for the long term. The chosen paradigm is that of a refcount-less cache, which accumulates
    items either manually, or from pre loaded files. The public API supports getting resources from the cache (if existant) through functions like `ye_image` and `ye_font`.
    There are also functions for manually adding resources to the cache, such as `ye_create_texture` and `ye_create_font`.
    The cache is only explicitely cleared when ye_clear_*_cache is called, which will free all cached resources of specified type.
    Normally, the scene manager will just clear the texture cache between scenes. The font and color cache will be cleared when the engine is shut down.

    This cache paradigm suits the intended use of the engine, as I envision loading all scene assets up front, and then clearing them when the scene is unloaded.
    To minimize the overhead of loading textures that might not need to be cleared on scene switch, `ye_swap_scene_cache` is provided to only prune textures that
    are not used in the new scene, and load any new textures that are needed.
*/

#ifndef YE_CACHE_H
#define YE_CACHE_H

#include <yoyoengine/yoyoengine.h>

/*
    Will iterate through a .yoyo scene file and cache all of its textures.

    NOTE:
    When implementing, consider that the scene file may reference prefabs from another file which could reference textures.
*/
void ye_pre_cache_scene(char *file_path){}

/*
    Will iterate through a .yoyo scene file and cache all of its fonts and colors, copying over any repeats from the old cache and deleting any that are not used.
*/
void ye_swap_scene_cache(char *file_path){}

/*
    Will iterate through a .yoyo style file and cache all of its fonts and colors.
*/
void ye_pre_cache_style(char *file_path){}

/*
    Will close all cached textures.
*/
void ye_clear_texture_cache(){}

/*
    Will close all cached fonts.
*/
void ye_clear_font_cache(){}

/*
    Will free all cached colors.
*/
void ye_clear_color_cache(){}





/*
    Will initialize the caches.
    After this, any color font or texture can be cached at any time, or pre cached before a scene load.
    This function will also parse styles.yoyo (if its not null) and load all fonts and colors.
*/
void ye_init_cache(char *styles_path){}

/*
    Will close all cached textures, fonts, and colors.
*/
void ye_shutdown_cache(){}





/*
    Linked lists
*/
struct ye_texture_node {
    SDL_Texture *texture;
    char *path;
    struct ye_texture_node *next;
};

struct ye_font_node {
    TTF_Font *font;
    char *name;
    int size;
    struct ye_font_node *next;
};

struct ye_color_node {
    SDL_Color color;
    char *name;
    struct ye_color_node *next;
};





/*
    API SPECIFIC TO CACHING ACCESS:
    The idea is accessing any sort of resource routes through here, and the cache layer will handle the loading.
    
    Colors and fonts must be created in styles.yoyo, (or manually before usage) and can be indexed by name. Fonts must be declared at a specific size in styles.yoyo.
*/

/*
    Returns the pointer to a cached texture, loading it if its not already cached.
*/
SDL_Texture * ye_image(char *path){}

/*
    Returns the pointer to a cached font based on name, will return NULL if not found.
*/
TTF_Font * ye_font(char *name){}

/*
    Caches a single color, will return NULL if not found
*/
SDL_Color * ye_color(char *name){}






/*
    Low level implementation that can also be used manually by developer.
    These functions will construct the underlying nodes and add them to the linked lists.
*/

/*
    Create a texture from path
*/
SDL_Texture * ye_create_texture(char *path){}

/*
    Create a font from path and size
*/
void ye_create_font(char *name, int size, char *path){}

/*
    Create a color from r g b a
*/
void ye_create_color(char *name, int r, int g, int b, int a){}

#endif