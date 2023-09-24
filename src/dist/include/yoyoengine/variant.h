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

#ifndef VARIANT_H
#define VARIANT_H

#include "uthash.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef enum {
    VARIANT_INT,
    VARIANT_FLOAT,
    VARIANT_STRING,
    VARIANT_COLOR,
    VARIANT_FONT,
    VARIANT_TEXTURE,
} VariantType;

typedef struct {
    VariantType type;
    int refcount;
    union {
        int intValue;
        float floatValue;
        char* stringValue;
        SDL_Color colorValue;
        TTF_Font* fontValue;
        SDL_Texture* textureValue;
    };
} Variant;

typedef struct {
    char* key;
    Variant value;
    UT_hash_handle hh;
} KeyValuePair;

typedef struct {
    KeyValuePair* map;
} VariantCollection;

VariantCollection* createVariantCollection();

void destroyVariantCollection(VariantCollection* collection);

void clearVariantCollection(VariantCollection* collection);

void addVariant(VariantCollection* collection, char* key, Variant variant);

Variant* getVariant(VariantCollection* collection, char* key);

void clearVariant(Variant* variant);

void removeVariant(VariantCollection* collection, char* key);

#endif /* VARIANT_H */
