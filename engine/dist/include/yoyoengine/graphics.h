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
 * @file graphics.h
 * @brief The engine API for handling graphics
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>

#include <SDL2/SDL_ttf.h>

#include <yoyoengine/yoyoengine.h>

/**
 * @brief Converts a float value to real pixel width.
 * @param in The float value to be converted.
 * @return The converted pixel width.
 */
int convertToRealPixelWidth(float in);

/**
 * @brief Converts a float value to real pixel height.
 * @param in The float value to be converted.
 * @return The converted pixel height.
 */
int convertToRealPixelHeight(float in);

/**
 * @brief Creates a SDL_Rect with real pixel values.
 * @param centered If true, the rectangle will be centered.
 * @param x The x-coordinate of the rectangle.
 * @param y The y-coordinate of the rectangle.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @return The created SDL_Rect.
 */
SDL_Rect createRealPixelRect(bool centered, float x, float y, float w, float h);

/**
 * @brief Loads a TTF_Font from a file.
 * @param pFontPath The path to the font file.
 * @param fontSize The size of the font.
 * @return The loaded TTF_Font.
 */
TTF_Font *ye_load_font(const char *pFontPath/*, int fontSize*/);

/**
 * @brief Creates a SDL_Texture from an image file.
 * @param pPath The path to the image file.
 * @return The created SDL_Texture, or NULL if the creation failed.
 */
SDL_Texture * ye_create_image_texture(const char *pPath);

/**
 * @brief Creates a text texture with an outline.
 * @return The created SDL_Texture.
 */
SDL_Texture *createTextTextureWithOutline();

/**
 * @brief Creates a text texture.
 * @param pText The text to be rendered.
 * @param pFont The font to be used.
 * @param pColor The color of the text.
 * @return The created SDL_Texture.
 */
SDL_Texture *createTextTexture(const char *pText, TTF_Font *pFont, SDL_Color *pColor);

/**
 * @brief Renders all elements.
 */
void ye_render_all();

/**
 * @brief Sets the viewport size.
 * @param screenWidth The width of the screen.
 * @param screenHeight The height of the screen.
 */
void setViewport(int screenWidth, int screenHeight);

/**
 * @brief Changes the window mode.
 * @param mode The new window mode.
 */
void changeWindowMode(Uint32 mode);

/**
 * @brief Changes the FPS cap.
 * @param cap The new FPS cap.
 */
void changeFPS(int cap);

/**
 * @brief Gets the current screen resolution.
 * @return The current screen resolution.
 */
struct ScreenSize getCurrentResolution();

/**
 * @brief Changes the screen resolution.
 * @param width The new width of the screen.
 * @param height The new height of the screen.
 */
void changeResolution(int width, int height);

/**
 * @brief Grabs the current screen resolution and recomputes the boxing as needed.
 */
void ye_recompute_boxing();

/**
 * @brief Initializes the graphics.
 */
void ye_init_graphics();

/**
 * @brief Shuts down the graphics.
 */
void ye_shutdown_graphics();

/**
 * @brief Gets the screen size.
 * @return The screen size.
 */
struct ScreenSize ye_get_screen_size();

#endif