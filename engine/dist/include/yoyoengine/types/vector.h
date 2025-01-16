/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/*
    A dynamic array type, in C!

    NOTE: Unstable, so we do not gaurantee the items to remain at the same address during lifetime of the vector.

    Usage:

    // Create vector of integers
    struct ye_vector* numbers = ye_new_vector(sizeof(int));
    
    // Push some numbers
    for(int i = 0; i < 5; i++) {
        ye_vector_push_back(numbers, &i);
    }
    
    // Insert at front
    int front_num = 42;
    ye_vector_push_front(numbers, &front_num);
    
    // Read and print values
    for(size_t i = 0; i < numbers->size; i++) {
        int* value = (int*)ye_vector_get(numbers, i);
        printf("Index %zu: %d\n", i, *value);
    }
    
    // Modify value
    int new_value = 100;
    ye_vector_set(numbers, 1, &new_value);
    
    // Remove element
    ye_vector_remove(numbers, 2);
    
    // Cleanup
    ye_free_vector(numbers);
*/

#ifndef YE_VECTOR_H
#define YE_VECTOR_H

#include <stdbool.h>
#include <stddef.h> // size_t

#include <yoyoengine/export.h>

#ifndef YE_VECTOR_INITIAL_CAPACITY
    #define YE_VECTOR_INITIAL_CAPACITY 2
#endif

#ifndef YE_VECTOR_SCALING_FACTOR
    #define YE_VECTOR_SCALING_FACTOR 2
#endif

/**
 * @brief A dynamic array type, in C!
 */
struct ye_vector {
    void    *_data;
    size_t  element_size;
    size_t  size;
    size_t  capacity;
};

/**
 * @brief Create a new vector.
 * 
 * @param element_size The size of the elements in the vector.
 * @return struct ye_vector* A pointer to the new vector.
 */
struct ye_vector * ye_new_vector(size_t element_size);

/**
 * @brief Free a vector.
 * 
 * @param vector The vector to free.
 */
void ye_free_vector(struct ye_vector *vector);

/**
 * @brief Reset a vector.
 * 
 * @param vector The vector to reset.
 */
void ye_vector_reset(struct ye_vector *vector);

/**
 * @brief Get an element from a vector.
 * 
 * @param vector The vector to get the element from.
 * @param index The index of the element to get.
 * @return void* A pointer to the element.
 */
void ye_vector_push_back(struct ye_vector *vector, void *element);

/**
 * @brief Push an element to the front of a vector.
 * 
 * @param vector The vector to push the element to.
 * @param element A pointer to the element to push.
 */
void ye_vector_push_front(struct ye_vector *vector, void *element);

/**
 * @brief Get an element from a vector.
 * 
 * @param vector The vector to get the element from.
 * @param index The index of the element to get.
 * @return void* A pointer to the element.
 */
void * ye_vector_get(struct ye_vector *vector, size_t index);

/**
 * @brief Set an element in a vector.
 * 
 * @param vector The vector to set the element in.
 * @param index The index of the element to set.
 * @param element A pointer to the element to set.
 */
void ye_vector_set(struct ye_vector *vector, size_t index, void *element);

/**
 * @brief Remove an element from a vector.
 * 
 * @param vector The vector to remove the element from.
 * @param index The index of the element to remove.
 */
void ye_vector_remove(struct ye_vector *vector, size_t index);

/**
 * @brief A macro to iterate over a vector. NOTE: "element" is a copy!!!
 * 
 * @param vector The vector to iterate over
 * @param type The type of elements in the vector
 * @param element The name for the element variable
 * 
 * @example
 * YE_VECTOR_FOR_EACH(my_vector, int, current) {
 *    printf("%d\n", current);
 * }
 */
#define YE_VECTOR_FOR_EACH(vector, type, element) \
    for (size_t _i __attribute__((unused)) = 0, _once = 1; \
        _once && _i < (vector)->size; \
        _i++) \
    for (type element = *(type*)ye_vector_get((vector), _i); \
        _once; \
        _once = 0)

/**
 * @brief A macro to iterate over a vector with index.
 * 
 * @param vector The vector to iterate over
 * @param type The type of elements in the vector
 * @param element The name for the element variable
 * @param idx The name for the index variable
 */
#define YE_VECTOR_FOR_EACH_INDEX(vector, type, element, idx) \
    for (size_t idx = 0, _once = 1; \
        _once && idx < (vector)->size; \
        idx++) \
    for (type element = *(type*)ye_vector_get((vector), idx); \
        _once; \
        _once = 0)

#endif // YE_VECTOR_H