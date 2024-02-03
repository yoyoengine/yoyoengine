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
 * @file cache.h
 * @brief The engine API for caching resources
 * 
 * Cache System:
 * The goal of this cache system is to be something that supports the engine for the long term. The chosen paradigm is that of a refcount-less cache, which accumulates
 * items either manually, or from pre loaded files. The public API supports getting resources from the cache (if existant) through functions like `ye_image` and `ye_font`.
 * There are also functions for manually adding resources to the cache, such as `ye_create_texture` and `ye_create_font`.
 * The cache is only explicitely cleared when ye_clear_*_cache is called, which will free all cached resources of specified type.
 * Normally, the scene manager will just clear the texture cache between scenes. The font and color cache will be cleared when the engine is shut down.
 * 
 * This cache paradigm suits the intended use of the engine, as I envision loading all scene assets up front, and then clearing them when the scene is unloaded.
 * To minimize the overhead of loading textures that might not need to be cleared on scene switch, `ye_pre_cache_scene` will reuse duplicate textures that have
 * already been cached when switching scenes.
 * 
 * Pre caching a scene will be opt-in, and will be done by the scene manager. In the case of very large scenes, the developer can still load textures on demand,
 * but will still need to find a way to pre-cache all their fonts and colors as needed.
 * 
 * The low level api does allow for deletion of individual resources, but I'm only including this for edge case compatibility, as I wont be using it for the purpose of
 * my games (yet).
 * 
 * The cache is implemented using UTHASH for simplicity and
 * 
 * The cache is very opt in, as you are only required to pass the pointers to resources when constructing ECS
 * components, meaning you could forgo the cache entirely and manually manage your memory. This is not recommended.
 * 
 * Tracking cache size:
 * 
 * This could be pretty easily done, but its not strictly necessary and will add overhead (unless optimized out)
 * It could be done like this if ever needed:
 * 
 * int ye_texture_size(SDL_Texture *texture){
 *    int w, h;
 *   SDL_QueryTexture(texture, NULL, NULL, &w, &h);
 *   return w * h * 4;
 * }
 * 
 * TODO:
 * - destruction of individual cache items
 * - caching of scene files
 * 
 * SHAPING:
 * scene.yoyo files can specify a styles.yoyo that it is dependant on... which scene manager can auto load
 */

#ifndef YE_CACHE_H
#define YE_CACHE_H

#include <yoyoengine/yoyoengine.h>

/**
 * @brief Pre-caches a scene.
 * 
 * Iterates through a .yoyo scene file and caches all of its textures. Any textures that can be reused from
 * the old scene will be carried over without reloading.
 * 
 * @param scene The scene to pre-cache.
 */
void ye_pre_cache_scene(json_t *scene);

/**
 * @brief Pre-caches styles.
 * 
 * Iterates through a .yoyo style file and caches all of its fonts and colors.
 * 
 * @param styles_path The path to the styles file.
 */
void ye_pre_cache_styles(const char *styles_path);

/**
 * @brief Clears the texture cache.
 * 
 * Closes all cached textures. Be careful doing this at runtime.
 */
void ye_clear_texture_cache();

/**
 * @brief Clears the font cache.
 * 
 * Closes all cached fonts. Be careful doing this at runtime.
 */
void ye_clear_font_cache();

/**
 * @brief Clears the color cache.
 * 
 * Frees all cached colors. Be careful doing this at runtime.
 */
void ye_clear_color_cache();

/**
 * @brief Initializes the caches.
 * 
 * After this, any color font or texture can be cached at any time, or pre-cached before a scene load.
 */
void ye_init_cache();

/**
 * @brief Shuts down the cache.
 * 
 * Closes all cached textures, fonts, and colors.
 */
void ye_shutdown_cache();

/**
 * @brief A node for a cached texture.
 */
struct ye_texture_node {
    SDL_Texture *texture; /**< The cached texture. */
    char *path; /**< The path to the texture. */
    UT_hash_handle hh; /**< The hash handle. */
};

/**
 * @brief A node for a cached font.
 */
struct ye_font_node {
    TTF_Font *font;     /**< The cached font. */
    char *name;         /**< The name of the font. */
    int size;           /**< The current size of the font. */
    UT_hash_handle hh;  /**< The hash handle. */
};

/**
 * @brief A node for a cached color.
 */
struct ye_color_node {
    SDL_Color color; /**< The cached color. */
    char *name; /**< The name of the color. */
    UT_hash_handle hh; /**< The hash handle. */
};





/**
 * @defgroup CacheAPI Cache API
 * @brief API specific to caching access.
 * 
 * The idea is accessing any sort of resource routes through here, and the cache layer will handle the loading.
 * Colors and fonts must be created in styles.yoyo, (or manually before usage) and can be indexed by name. Fonts must be declared at a specific size in styles.yoyo.
 * @{
 */

/**
 * @brief Returns the pointer to a cached texture, loading it if its not already cached.
 * @param path The path to the texture.
 * @return The cached texture.
 */
SDL_Texture * ye_image(const char *path);

/**
 * @brief Returns the pointer to a cached font based on name and size, returning a fallback default font if not found.
 * @param name The name of the font.
 * @return The cached font.
 */
TTF_Font * ye_font(const char *name, int size);

/**
 * @brief Caches a single color, returning a fallback default color if not found.
 * @param name The name of the color.
 * @return The cached color.
 */
SDL_Color * ye_color(const char *name);

/** @} */ // end of CacheAPI

/**
 * @defgroup CacheLowLevel Cache Low Level
 * @brief Low level implementation that can also be used manually by developer.
 * These functions will construct the underlying nodes and add them to the hash cache.
 * @{
 */

/**
 * @brief Manually cache an already loaded texture with a provided key
 * 
 * @param texture Texture to cache
 * @param key A UNIQUE key to cache the texture under
 * 
 * @note this is useful to make sure engine takes care of cleaning up a texture automatically
 */
void ye_cache_texture_manual(SDL_Texture *texture, const char *key);

/**
 * @brief Create a texture from path.
 * @param path The path to the texture.
 * @return The cached texture.
 */
SDL_Texture * ye_cache_texture(const char *path);

/**
 * @brief Manually cache an already loaded font with a provided key
 * 
 * @param name The name of the font key
 * @param font The font to cache
 */
TTF_Font * ye_cache_font_manual(const char *name, TTF_Font *font);

/**
 * @brief Create a font from name, size, and path.
 * @param name The name of the font.
 * @param size The size of the font.
 * @param path The path to the font.
 * @return The cached font.
 */
TTF_Font * ye_cache_font(const char *name/*, int size*/, const char *path);

/**
 * @brief Cache a SDL_Color.
 * @param name The name of the color.
 * @param color The color to cache.
 * @return The cached color.
 */
SDL_Color * ye_cache_color(const char *name, SDL_Color color);

/**
 * @brief Destroy a cached texture from path.
 * @param path The path to the texture.
 */
void ye_destroy_texture(const char *path);

/**
 * @brief Destroy a cached font from name.
 * @param name The name of the font.
 */
void ye_destroy_font(const char *name);

/**
 * @brief Destroy a cached color from name.
 * @param name The name of the color.
 */
void ye_destroy_color(const char *name);

/** @} */ // end of CacheLowLevel

#endif