/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YE_CONFIG_H
#define YE_CONFIG_H

#include <jansson.h>

/**
 * @brief Retrieves an int from a config, settings and returning a default value if nonexistant
 * 
 * @param config The json_t object to read from
 * @param key The item key
 * @param default_value The default value to return if the key is not found
 * @return int The value of the key, or the default value if the key is not found
 */
int ye_config_int(json_t* config, const char* key, int default_value);

/**
 * @brief Retrieves a float from a config, settings and returning a default value if nonexistant
 * 
 * @param config The json_t object to read from
 * @param key The item key
 * @param default_value The default value to return if the key is not found
 * @return float The value of the key, or the default value if the key is not found
 */
float ye_config_float(json_t* config, const char* key, float default_value);

/**
 * @brief Retrieves a string from a config, settings and returning a default value if nonexistant
 * 
 * @param config The json_t object to read from
 * @param key The item key
 * @param default_value The default value to return if the key is not found
 * @return char* The value of the key, or the default value if the key is not found. YOU MUST FREE THIS!
 */
char* ye_config_string(json_t* config, const char* key, const char* default_value);

/**
 * @brief Retrieves a bool from a config, settings and returning a default value if nonexistant
 * 
 * @param config The json_t object to read from
 * @param key The item key
 * @param default_value The default value to return if the key is not found
 * @return bool The value of the key, or the default value if the key is not found
 */
bool ye_config_bool(json_t* config, const char* key, bool default_value);

#endif