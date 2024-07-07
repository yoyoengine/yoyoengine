/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file renderer.h
 * @brief ECS Renderer component
 */

#ifndef YE_RENDERER_H
#define YE_RENDERER_H

#include <stdbool.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include <yoyoengine/utils.h>

/**
 * @enum ye_component_renderer_type
 * @brief Enum to store different unique types of renderers.
 * This is how we identify steps needed to render different types of entities.
 * Ex: animation renderer knows it needs to increment frames, text renderer knows how to reconstruct text, etc
 */
enum ye_component_renderer_type {
    YE_RENDERER_TYPE_TEXT,
    YE_RENDERER_TYPE_TEXT_OUTLINED,
    YE_RENDERER_TYPE_IMAGE,
    YE_RENDERER_TYPE_ANIMATION,
    YE_RENDERER_TYPE_TILEMAP_TILE
};

/**
 * @brief A structure to represent a component renderer.
 */
struct ye_component_renderer {
    bool active;    ///< controls whether system will act upon this component

    SDL_Texture *texture;   ///< texture to render. For tilemaps this will be the full image even if only a portion is rendered

    enum ye_component_renderer_type type;   ///< denotes which renderer is needed for this entity

    int alpha;  ///< alpha of texture

    bool relative; ///< whether or not this comp is relative to a parent transform
    struct ye_rectf rect;

    enum ye_alignment alignment;    ///< alignment of entity within its bounds
    bool preserve_original_size;    ///< whether or not to preserve the original size of the entity when fitting bounds
    int z;                          ///< layer the entity sits on
    SDL_Point center;               ///< center of rotation
    float rotation;                 ///< rotation of entity in degrees

    struct ye_rectf computed_pos;   ///< the computed pos to display the entity at

    bool flipped_x;
    bool flipped_y;

    union renderer_impl{ ///< hold the data for the specific renderer type
        struct ye_component_renderer_text *text;
        struct ye_component_renderer_text_outlined *text_outlined;
        struct ye_component_renderer_image *image;
        struct ye_component_renderer_animation *animation;
        struct ye_component_renderer_tilemap_tile *tile;
    } renderer_impl;

    bool lock_aspect_ratio; ///< locks the rect aspect ratio
};

/**
 * @brief A structure to represent an image renderer.
 */
struct ye_component_renderer_image {
    char *src;  ///< path to image
};

/**
 * @brief A structure to represent a text renderer.
 */
struct ye_component_renderer_text {
    char *text;         ///< text to render
    char *font_name;    ///< name of font to use
    int font_size;      ///< size of font to use
    char *color_name;   ///< name of the color to use
    TTF_Font *font;     ///< font to use
    SDL_Color *color;   ///< color of text
    int wrap_width;     ///< if >0 then wrap text to this width (in pixels
};

/**
 * @brief A structure to represent an outlined text renderer.
 */
struct ye_component_renderer_text_outlined {
    char *text;                 ///< text to render
    int outline_size;           ///< size of text outline
    char *font_name;            ///< name of font to use
    int font_size;              ///< size of font to use
    char *color_name;           ///< name of the color to use
    char *outline_color_name;   ///< name of the color to use for the outline
    TTF_Font *font;             ///< font to use
    SDL_Color *color;           ///< color of text
    SDL_Color *outline_color;   ///< color of text outline
    int wrap_width;             ///< if >0 then wrap text to this width (in pixels)
};

/**
 * @brief A structure to represent an animation renderer.
 */
struct ye_component_renderer_animation {
    char *animation_handle;     ///< resource for animation map image
    char *meta_file;            ///< meta file for animation details

    size_t frame_count;         ///< number of frames in animation

    int frame_delay;            ///< delay between frames in ms
    int loops;                  ///< number of loops, -1 for infinite
    bool paused;                ///< whether or not the animation is paused

    int frame_width;            ///< width of each frame
    int frame_height;           ///< height of each frame

    // meta for engine:
    int last_updated;           ///< SDL_GetTicks() last frame advance
    int current_frame_index;    ///< current frame index
};

/**
 * @brief A structure to represent a tilemap tile
 */
struct ye_component_renderer_tilemap_tile {
    char *handle;   ///< handle to tilemap source image (from loose or pack)
    SDL_Rect src;   ///< source rect of tile
};

/**
 * @brief Will refresh the values and texture of a renderer component based on its fields.
 * @param entity The entity to refresh.
 */
void ye_update_renderer_component(struct ye_entity *entity);

/**
 * @brief Adds a renderer component to an entity.
 * @note Do not use this directly unless you know what you're doing.
 * @param entity The entity to add the renderer component to.
 * @param type The type of the renderer component.
 * @param z The z-index of the renderer component.
 * @param data A void pointer to a struct of matching type.
 */
void ye_add_renderer_component(struct ye_entity *entity, enum ye_component_renderer_type type, int z, void *data);

/**
 * @brief  Adds an image renderer component to an entity.
 * @param entity The entity to add the image renderer component to.
 * @param z The z-index of the image renderer component.
 * @param src The source of the image.
 */
void ye_add_image_renderer_component(struct ye_entity *entity, int z,const char *src);

/**
 * @brief Adds an image renderer component to an entity.
 * @param entity The entity to add the image renderer component to.
 * @param z The z-index of the image renderer component.
 * @param texture The texture to use for the image.
 * @note This function is used for preloaded textures.
 */
void ye_add_image_renderer_component_preloaded(struct ye_entity *entity, int z, SDL_Texture *texture);

/**
 * @brief Temporarily adds a text renderer component to an entity.
 * @param entity The entity to add the text renderer component to.
 * @param z The z-index of the text renderer component.
 * @param text The text to render.
 * @param font The font to use for rendering the text.
 * @param font_size The size of the font to use for rendering the text.
 * @param color The color to use for rendering the text.
 */
void ye_add_text_renderer_component(struct ye_entity *entity, int z, const char *text, const char *font, int font_size, const char *color, int wrap_width);

/**
 * @brief Temporarily adds an outlined text renderer component to an entity.
 * @param entity The entity to add the outlined text renderer component to.
 * @param z The z-index of the outlined text renderer component.
 * @param text The text to render.
 * @param font The font to use for rendering the text.
 * @param font_size The size of the font to use for rendering the text.
 * @param color The color to use for rendering the text.
 * @param outline_color The color to use for the outline of the text.
 * @param outline_size The size of the outline.
 */
void ye_add_text_outlined_renderer_component(struct ye_entity *entity, int z, const char *text, const char *font, int font_size, const char *color, const char *outline_color, int outline_size, int wrap_width);

/**
 * @brief Temporarily adds an animation renderer component to an entity.
 * @param entity The entity to add the animation renderer component to.
 * @param z The z-index of the animation renderer component.
 * @param meta_file The meta file to use for loading animation details.
 */
void ye_add_animation_renderer_component(struct ye_entity *entity, int z, const char *meta_file);

/**
 * @brief Adds a tilemap tile renderer component to an entity.
 * 
 * @param entity The entity to add the tilemap tile renderer component to.
 * @param z The z-index of the tilemap tile renderer component.
 * @param handle The handle to the tilemap source image (from loose or pack).
 * @param src The source rect of the tile.
 */
void ye_add_tilemap_renderer_component(struct ye_entity *entity, int z, const char * handle, SDL_Rect src);

/**
 * @brief Removes a renderer component from an entity.
 * @param entity The entity to remove the renderer component from.
 */
void ye_remove_renderer_component(struct ye_entity *entity);

/**
 * @brief Handles the rendering system.
 * @param renderer The SDL renderer to use.
 * 
 * Renderer system
 * 
 * Acts upon the list of tracked entities with renderers and paints them to the screen.
 * 
 * Uses the transform component to determine where to paint the entity.
 * Skips entity if there is no active transform or renderer is inactive
 * 
 * This system will paint relative to the active camera, and occlude anything
 * outside of the active camera's view field
 * 
 * TODO:
 * - fulcrum cull rotated entities
 * 
 * NOTES:
 * - Initially I tried some weird really complicated impl, which I will share details
 * of below, for future me who will likely need these in the future.
 * For now, we literally just check an intersection of the object and the camera, and if it exists we
 * copy the object to the renderer offset by the camera position.
 * In the future we might want to return to the weird clip plane system, but for now this is fine.
 * - I'm making a REALLY stupid assumption that we dont really scale the camera viewfield outside of the
 * window resolution. I dont want to deal with the paint issues that i need to solve to truly paint from the cameras POV
 * 
 * Some functions and relevant snippets to the old system:
 * - SDL_RenderSetClipRect(renderer, &visibleRect);
 * - SDL_Rect rect = {0,0,640,320}; SDL_RenderSetViewport(pRenderer, &rect);
 */
void ye_system_renderer(SDL_Renderer *renderer);

#endif