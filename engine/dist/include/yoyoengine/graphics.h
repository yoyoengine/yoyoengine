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

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>

#include <SDL2/SDL_ttf.h>

#include <yoyoengine/yoyoengine.h>

int convertToRealPixelWidth(float in);

int convertToRealPixelHeight(float in);

SDL_Rect createRealPixelRect(bool centered, float x, float y, float w, float h);

TTF_Font *ye_load_font(const char *pFontPath, int fontSize);

// Create a texture from image path, returns NULL for failure
SDL_Texture * ye_create_image_texture(const char *pPath);

SDL_Texture *createTextTextureWithOutline();

SDL_Texture *createTextTexture(const char *pText, TTF_Font *pFont, SDL_Color *pColor);

void renderAll();

void setViewport(int screenWidth, int screenHeight);

void changeWindowMode(Uint32 mode);

void changeFPS(int cap);

struct ScreenSize getCurrentResolution();

void changeResolution(int width, int height);

void ye_init_graphics();

void ye_shutdown_graphics();

struct ScreenSize ye_get_screen_size();

#endif