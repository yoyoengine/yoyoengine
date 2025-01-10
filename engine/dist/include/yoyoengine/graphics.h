/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file graphics.h
 * @brief The engine API for handling graphics
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <yoyoengine/export.h>

#include <SDL.h>           // SDL_Rect, SDL_Texture, SDL_Color
#include <stdbool.h>            // bool
#include <SDL_ttf.h>       // TTF_Font
#include <yoyoengine/engine.h>  // struct ScreenSize

/**
 * @brief Converts a float value to real pixel width.
 * @param in The float value to be converted.
 * @return The converted pixel width.
 */
YE_API int convertToRealPixelWidth(float in);

/**
 * @brief Converts a float value to real pixel height.
 * @param in The float value to be converted.
 * @return The converted pixel height.
 */
YE_API int convertToRealPixelHeight(float in);

/**
 * @brief Creates a SDL_Rect with real pixel values.
 * @param centered If true, the rectangle will be centered.
 * @param x The x-coordinate of the rectangle.
 * @param y The y-coordinate of the rectangle.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @return The created SDL_Rect.
 */
YE_API SDL_Rect createRealPixelRect(bool centered, float x, float y, float w, float h);

/**
 * @brief Loads a TTF_Font from a file.
 * @param pFontPath The path to the font file.
 * @param fontSize The size of the font.
 * @return The loaded TTF_Font.
 */
YE_API TTF_Font *ye_load_font(const char *pFontPath/*, int fontSize*/);

/**
 * @brief Creates a SDL_Texture from an image file.
 * @param pPath The path to the image file.
 * @return The created SDL_Texture, or NULL if the creation failed.
 */
YE_API SDL_Texture * ye_create_image_texture(const char *pPath);

/**
 * @brief Creates a text texture with an outline.
 * @return The created SDL_Texture.
 */
YE_API SDL_Texture *createTextTextureWithOutline(const char *pText, int width, TTF_Font *pFont, SDL_Color *pColor, SDL_Color *pOutlineColor);

/*
    Untested rn.
*/
YE_API SDL_Texture *createTextTextureWithOutlineWrapped(const char *pText, int width, TTF_Font *pFont, SDL_Color *pColor, SDL_Color *pOutlineColor, int wrapLength);

/**
 * @brief Creates a text texture.
 * @param pText The text to be rendered.
 * @param pFont The font to be used.
 * @param pColor The color of the text.
 * @return The created SDL_Texture.
 */
YE_API SDL_Texture *createTextTexture(const char *pText, TTF_Font *pFont, SDL_Color *pColor);

/**
 * @brief Creates a text texture with wrapping.
 * @param pText The text to be rendered.
 * @param pFont The font to be used.
 * @param pColor The color of the text.
 * @param wrapLength The length to wrap the text at.
 * @return The created SDL_Texture.
 */
YE_API SDL_Texture *createTextTextureWrapped(const char *pText, TTF_Font *pFont, SDL_Color *pColor, int wrapLength);

/**
 * @brief Renders all elements.
 */
YE_API void ye_render_all();

/**
 * @brief Sets the viewport size.
 * @param screenWidth The width of the screen.
 * @param screenHeight The height of the screen.
 */
YE_API void setViewport(int screenWidth, int screenHeight);

/**
 * @brief Changes the window mode.
 * @param mode The new window mode.
 */
YE_API void changeWindowMode(Uint32 mode);

/**
 * @brief Changes the FPS cap.
 * @param cap The new FPS cap.
 */
YE_API void changeFPS(int cap);

/**
 * @brief Gets the current screen resolution.
 * @return The current screen resolution.
 */
YE_API struct ScreenSize getCurrentResolution();

/**
 * @brief Changes the screen resolution.
 * @param width The new width of the screen.
 * @param height The new height of the screen.
 */
YE_API void changeResolution(int width, int height);

/**
 * @brief Grabs the current screen resolution and recomputes the boxing as needed.
 */
YE_API void ye_recompute_boxing();

/**
 * @brief Initializes the graphics.
 */
YE_API void ye_init_graphics();

/**
 * @brief Shuts down the graphics.
 */
YE_API void ye_shutdown_graphics();

/**
 * @brief Gets the screen size.
 * @return The screen size.
 */
YE_API struct ScreenSize ye_get_screen_size();

/*
    GRAPHICS API:
*/

/**
 * @brief Sets the window mode
 * 
 * @param mode The SDL window mode to change to
 */
YE_API void ye_set_window_mode(int mode);

#endif