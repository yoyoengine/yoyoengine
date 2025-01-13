/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <string.h>
#include <stdlib.h>

#include <yoyoengine/logging.h>
#include <yoyoengine/types/vector.h>

void _resize_as_needed(struct ye_vector *vector) {
    if(!vector) {
        ye_logf(YE_LL_ERROR, "ye_vector _resize_as_needed: recieved NULL vector!\n");
        return;
    }

    if(vector->size >= vector->capacity) {
        vector->capacity *= YE_VECTOR_SCALING_FACTOR;
        vector->_data = realloc(vector->_data, vector->element_size * vector->capacity);
    }

    if(!vector->_data) {
        ye_logf(YE_LL_ERROR, "ye_vector _resize_as_needed: Failed to reallocate memory for vector!\n");
        return;
    }
}

struct ye_vector * ye_new_vector(size_t element_size) {
    if(element_size <= 0) {
        ye_logf(YE_LL_ERROR, "ye_new_vector: recieved element_size <= 0!\n");
        return NULL;
    }

    struct ye_vector *vector = malloc(sizeof(struct ye_vector));
    if(!vector) {
        ye_logf(YE_LL_ERROR, "ye_new_vector: failed to allocate memory for vector!\n");
        return NULL;
    }

    vector->_data = malloc(element_size * YE_VECTOR_INITIAL_CAPACITY);
    vector->element_size = element_size;
    vector->size = 0;
    vector->capacity = YE_VECTOR_INITIAL_CAPACITY;
    return vector;
}

void ye_free_vector(struct ye_vector *vector) {
    if(!vector) {
        ye_logf(YE_LL_WARNING, "ye_free_vector: recieved NULL vector!\n");
        return;
    }
    
    free(vector->_data);
    free(vector);
}

void ye_vector_reset(struct ye_vector *vector) {
    if(!vector) {
        ye_logf(YE_LL_ERROR, "ye_vector_reset: recieved NULL vector!\n");
        return;
    }

    vector->size = 0;
}

void ye_vector_push_back(struct ye_vector *vector, void *element) {
    if(!vector) {
        ye_logf(YE_LL_ERROR, "ye_vector_push_back: recieved NULL vector!\n");
        return;
    }

    _resize_as_needed(vector);

    memcpy((char *)vector->_data + vector->size * vector->element_size, element, vector->element_size);
    vector->size++;
}

void ye_vector_push_front(struct ye_vector *vector, void *element) {
    if(!vector) {
        ye_logf(YE_LL_ERROR, "ye_vector_push_front: recieved NULL vector!\n");
        return;
    }

    _resize_as_needed(vector);

    for(size_t i = vector->size; i > 0; i--) {
        memcpy((char *)vector->_data + i * vector->element_size, (char *)vector->_data + (i - 1) * vector->element_size, vector->element_size);
    }

    memcpy(vector->_data, element, vector->element_size);
    vector->size++;
}

void * ye_vector_get(struct ye_vector *vector, size_t index) {
    if(!vector) {
        ye_logf(YE_LL_ERROR, "ye_vector_get: recieved NULL vector!\n");
        return NULL;
    }

    if(index >= vector->size) {
        ye_logf(YE_LL_ERROR, "ye_vector_get: index out of bounds!\n");
        return NULL;
    }

    return (char *)vector->_data + index * vector->element_size;
}

void ye_vector_set(struct ye_vector *vector, size_t index, void *element) {
    if(!vector) {
        ye_logf(YE_LL_ERROR, "ye_vector_set: recieved NULL vector!\n");
        return;
    }

    if(index >= vector->size) {
        ye_logf(YE_LL_ERROR, "ye_vector_set: index out of bounds!\n");
        return;
    }

    memcpy((char *)vector->_data + index * vector->element_size, element, vector->element_size);
}

void ye_vector_remove(struct ye_vector *vector, size_t index) {
    if(!vector) {
        ye_logf(YE_LL_ERROR, "ye_vector_remove: recieved NULL vector!\n");
        return;
    }

    if(index >= vector->size) {
        ye_logf(YE_LL_ERROR, "ye_vector_remove: index out of bounds!\n");
        return;
    }

    for(size_t i = index; i < vector->size - 1; i++) {
        memcpy((char *)vector->_data + i * vector->element_size, (char *)vector->_data + (i + 1) * vector->element_size, vector->element_size);
    }

    vector->size--;
}