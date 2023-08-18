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
