#include "lib/variant.h"
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