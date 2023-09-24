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

#include <stdlib.h>
#include <string.h>

VariantCollection* createVariantCollection() {
    VariantCollection* collection = malloc(sizeof(VariantCollection));
    collection->map = NULL;
    return collection;
}

void destroyVariantCollection(VariantCollection* collection) {
    KeyValuePair* pair, *tmp;

    HASH_ITER(hh, collection->map, pair, tmp) {
        HASH_DEL(collection->map, pair);
        clearVariant(&pair->value);
        free(pair->key);
        free(pair);
    }

    free(collection);
}

void clearVariantCollection(VariantCollection* collection) {
    KeyValuePair* pair, *tmp;

    HASH_ITER(hh, collection->map, pair, tmp) {
        HASH_DEL(collection->map, pair);
        clearVariant(&pair->value);
        free(pair->key);
        free(pair);
    }
}

void addVariant(VariantCollection* collection, char* key, Variant variant) {
    KeyValuePair* pair = malloc(sizeof(KeyValuePair));
    pair->key = strdup(key);
    pair->value = variant;
    HASH_ADD_KEYPTR(hh, collection->map, pair->key, strlen(pair->key), pair);
}

Variant* getVariant(VariantCollection* collection, char* key) {
    KeyValuePair* pair;
    HASH_FIND_STR(collection->map, key, pair);
    if (pair != NULL)
        return &pair->value;
    return NULL;
}

void clearVariant(Variant* variant) {
    if (variant->type == VARIANT_STRING) {
        free(variant->stringValue);
    }
    else if(variant->type == VARIANT_FONT) {
        TTF_CloseFont(variant->fontValue);
    }
    else if(variant->type == VARIANT_TEXTURE) {
        // printf("freeing variant texture with refcount: %d\n",variant->refcount);
        SDL_DestroyTexture(variant->textureValue);
    }
}

/*
    Remove a variant from its collection by key
*/
void removeVariant(VariantCollection* collection, char* key) {
    KeyValuePair* pair;
    HASH_FIND_STR(collection->map, key, pair);

    if (pair != NULL) {
        HASH_DEL(collection->map, pair);
        clearVariant(&pair->value);
        free(pair->key);
        free(pair);
    }
}