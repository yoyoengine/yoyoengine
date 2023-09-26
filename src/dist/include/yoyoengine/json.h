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

#ifndef YE_JSON_H
#define YE_JSON_H

#include <yoyoengine/yoyoengine.h>
#include <jansson/jansson.h>
#include <stdbool.h>

/*
    json wrapper

    This is a wrapper around the jansson library that provides a more convenient interface as well
    as validation and error checking. If you try to access a value that does not exist you will recieve
    false, with the output ptr being NULL as well as a ye_logf warning.

    Functions take in the json_t * to act upon, a key to index a value via, and an output ptr to assign
    the value to. If the key is not found, the function will return false and the output ptr will be
    NULL. If the key is found, the function will return true and the output ptr will be assigned the
    value.
*/

/*
    Returns the content of a json file (NULL if it doesn't exist)
    This pointer is a new json_t object with refcount 1 and must be freed after use with jhson_decref
*/
json_t* ye_json_read(const char* path);

/*
    Writes a json_t object to a file
    Returns 0 on success, -1 on failure
*/
int ye_json_write(const char* path, json_t* json);

/*
    Debugging function to log a json_t object
*/
void ye_json_log(json_t* json);

/*
    Merge two json_t objects (overwrite the first with the second if conflicts occur)
    This will modify the first json_t object (without upping its refcount). Returns 0 on success, -1 on failure
*/
int ye_json_merge(json_t* first, json_t* second);

/*
    Checks if a json_t object has a key
*/
bool ye_json_has_key(json_t* json, const char* key);

/*
    Extract an int from a json_t by key, assigning the passed int to it
    Returns true if extraction was successful, false otherwise.
*/
bool ye_json_int(json_t* json, const char* key, int *out);

/*
    Extract a double from a json_t by key, assigning the passed double to it
    Returns true if extraction was successful, false otherwise.
*/
bool ye_json_double(json_t* json, const char* key, double *out);

/*
    Extract a bool from a json_t by key, assigning the passed bool to it
    Returns true if extraction was successful, false otherwise.
*/
bool ye_json_bool(json_t* json, const char* key, bool *out);

/*
    Extract a string from a json_t by key, assigning the passed string to it
    Returns true if extraction was successful, false otherwise.
*/
bool ye_json_string(json_t* json, const char* key, const char **out);

/*
    Extract a json_t from a json_t by key, assigning the passed json_t to it
    Returns true if extraction was successful, false otherwise.
*/
bool ye_json_object(json_t* json, const char* key, json_t **out);

/*
    Extract a json_t from a json_t by key, assigning the passed json_t to it
    Returns true if extraction was successful, false otherwise.
*/
bool ye_json_array(json_t* json, const char* key, json_t **out);

/*
    Extract an int from a json_t by index, assigning the passed int to it
    Returns true if extraction was successful, false otherwise.
*/
bool ye_json_arr_int(json_t* json, int index, int *out);

/*
    Extract a double from a json_t by index, assigning the passed double to it
    Returns true if extraction was successful, false otherwise.
*/
bool ye_json_arr_double(json_t* json, int index, double *out);

/*
    Extract a bool from a json_t by index, assigning the passed bool to it
    Returns true if extraction was successful, false otherwise.
*/
bool ye_json_arr_bool(json_t* json, int index, bool *out);

/*
    Extract a string from a json_t by index, assigning the passed string to it
    Returns true if extraction was successful, false otherwise.
*/
bool ye_json_arr_string(json_t* json, int index, const char **out);

/*
    Extract a json_t object from a json_t by index, assigning the passed json_t to it
    Returns true if extraction was successful, false otherwise.
*/
bool ye_json_arr_object(json_t* json, int index, json_t **out);

/*
    Extract a json_t array from a json_t by index, assigning the passed json_t to it
    Returns true if extraction was successful, false otherwise.
*/
bool ye_json_arr_array(json_t* json, int index, json_t **out);

#endif