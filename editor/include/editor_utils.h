/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef EDITOR_UTILS_H
#define EDITOR_UTILS_H

#include <stdbool.h>

/**
 * @brief Opens a file or URL in the system's default application.
 * 
 * @param url_or_file_path The URL or file path to open
 */
void editor_open_in_system(const char *url_or_file_path);

/**
 * @brief Touches a file with the given content.
 * 
 * @param file_path The path to the file (the folder will be created if it doesn't exist)
 * @param content The content to write to the file, if any
 */
void editor_touch_file(const char *file_path, const char *content);

/**
 * @brief Checks if a file exists.
 * 
 * @param file_path The path to the file
 * @return true if the file exists, false otherwise
 */
bool editor_file_exists(const char *file_path);

#endif // EDITOR_UTILS_H