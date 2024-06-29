/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

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