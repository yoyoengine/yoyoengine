/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file json.h
 * @brief The engine API for working with json data, built on top of jansson
 * 
 * json wrapper
 * 
 * This is a light wrapper around the jansson library that provides a more convenient interface as well
 * as validation and error checking. If you try to access a value that does not exist you will recieve
 * false, with the output ptr not being modified as well as a ye_logf warning.
 * 
 * Functions take in the json_t * to act upon, a key to index a value via, and an output ptr to assign
 * the value to. If the key is not found, the function will return false and the output ptr will be
 * NULL. If the key is found, the function will return true and the output ptr will be assigned the value.
 * 
 * Some reccomended convention:
 * name any root json_t objects in UPPERCASE, and ensure to json_decref them when you are done with them
 */

#ifndef YE_JSON_H
#define YE_JSON_H

#include <yoyoengine/export.h>

#include <jansson.h>
#include <stdbool.h>

/**
 * @brief Reads a JSON file and returns its content.
 * @param path The path to the JSON file.
 * @return A new json_t object with refcount 1. Returns NULL if the file doesn't exist.
 * @note The returned pointer must be freed after use with json_decref.
 */
YE_API json_t* ye_json_read(const char* path);

/**
 * @brief Writes a json_t object to a file.
 * @param path The path to the file.
 * @param json The json_t object to write.
 * @return 0 on success, -1 on failure.
 */
YE_API int ye_json_write(const char* path, json_t* json);

/**
 * @brief Logs a json_t object for debugging purposes.
 * @param json The json_t object to log.
 */
YE_API void ye_json_log(json_t* json);

/**
 * @brief Merges two json_t objects, overwriting the first with the second if conflicts occur.
 * @param first The first json_t object.
 * @param second The second json_t object.
 * @return 0 on success, -1 on failure.
 * @note This will modify the first json_t object without increasing its refcount.
 */
YE_API int ye_json_merge(json_t* first, json_t* second);

/**
 * @brief Merges two json_t objects, but only updates values of existing keys in the first object.
 * @param first The first json_t object.
 * @param second The second json_t object.
 * @return 0 on success, -1 on failure.
 */
YE_API int ye_json_merge_existing(json_t* first, json_t* second);

/**
 * @brief Merges two json_t objects, but only creates new keys, existing keys are not updated.
 * @param first The first json_t object.
 * @param second The second json_t object.
 * @return 0 on success, -1 on failure.
 */
YE_API int ye_json_merge_missing(json_t* first, json_t* second);

/**
 * @brief Checks if a json_t object has a specific key.
 * @param json The json_t object.
 * @param key The key to check.
 * @return true if the key exists, false otherwise.
 */
YE_API bool ye_json_has_key(json_t* json, const char* key);

/**
 * @brief Extracts an int from a json_t object by key.
 * @param json The json_t object.
 * @param key The key to extract the int from.
 * @param out The pointer to assign the extracted int to.
 * @return true if extraction was successful, false otherwise.
 */
YE_API bool ye_json_int(json_t* json, const char* key, int *out);

/**
 * @brief Extract a float from a json_t by key, assigning the passed float to it
 * @param json The JSON object
 * @param key The key to extract the float from
 * @param out The output float
 * @return true if extraction was successful, false otherwise
 */
YE_API bool ye_json_float(json_t* json, const char* key, float *out);

/**
 * @brief Extract a bool from a json_t by key, assigning the passed bool to it
 * @param json The JSON object
 * @param key The key to extract the bool from
 * @param out The output bool
 * @return true if extraction was successful, false otherwise
 */
YE_API bool ye_json_bool(json_t* json, const char* key, bool *out);

/**
 * @brief Extract a string from a json_t by key, assigning the passed string to it
 * @note THIS FUNCTION SETS const char **out TO BE A REFERENCE TO THE STRING IN JANSSON, IF THIS NEEDS TO PERSIST
 * YOU NEED TO COPY OR DUPLICATE ITS MEMORY
 * @param json The JSON object
 * @param key The key to extract the string from
 * @param out The output string
 * @return true if extraction was successful, false otherwise
 */
YE_API bool ye_json_string(json_t* json, const char* key, const char **out);

/**
 * @brief Extract a json_t from a json_t by key, assigning the passed json_t to it
 * @param json The JSON object
 * @param key The key to extract the json_t from
 * @param out The output json_t
 * @return true if extraction was successful, false otherwise
 */
YE_API bool ye_json_object(json_t* json, const char* key, json_t **out);

/**
 * @brief Extract a json_t from a json_t by key, assigning the passed json_t to it
 * @note This function is practically the same as object, but provides a sanity check for array type
 * @param json The JSON object
 * @param key The key to extract the json_t from
 * @param out The output json_t
 * @return true if extraction was successful, false otherwise
 */
YE_API bool ye_json_array(json_t* json, const char* key, json_t **out);

/**
 * @brief Extract an int from a json_t by index, assigning the passed int to it
 * @param json The JSON object
 * @param index The index to extract the int from
 * @param out The output int
 * @return true if extraction was successful, false otherwise
 */
YE_API bool ye_json_arr_int(json_t* json, int index, int *out);

/**
 * @brief Extracts a double from a json_t object by index.
 * @param json The json_t object.
 * @param index The index to extract the double from.
 * @param out The pointer to assign the extracted double to.
 * @return true if extraction was successful, false otherwise.
 */
YE_API bool ye_json_arr_double(json_t* json, int index, double *out);

/**
 * @brief Extracts a bool from a json_t object by index.
 * @param json The json_t object.
 * @param index The index to extract the bool from.
 * @param out The pointer to assign the extracted bool to.
 * @return true if extraction was successful, false otherwise.
 */
YE_API bool ye_json_arr_bool(json_t* json, int index, bool *out);

/**
 * @brief Extracts a string from a json_t object by index.
 * @param json The json_t object.
 * @param index The index to extract the string from.
 * @param out The pointer to assign the extracted string to.
 * @return true if extraction was successful, false otherwise.
 * @note This function sets const char **out to be a reference to the string in Jansson. If this needs to persist, you need to copy or duplicate its memory.
 */
YE_API bool ye_json_arr_string(json_t* json, int index, const char **out);

/**
 * @brief Extracts a json_t object from a json_t object by index.
 * @param json The json_t object.
 * @param index The index to extract the json_t object from.
 * @param out The pointer to assign the extracted json_t object to.
 * @return true if extraction was successful, false otherwise.
 */
YE_API bool ye_json_arr_object(json_t* json, int index, json_t **out);

/**
 * @brief Extracts a json_t array from a json_t object by index.
 * @param json The json_t object.
 * @param index The index to extract the json_t array from.
 * @param out The pointer to assign the extracted json_t array to.
 * @return true if extraction was successful, false otherwise.
 */
YE_API bool ye_json_arr_array(json_t* json, int index, json_t **out);

#endif