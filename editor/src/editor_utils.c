/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stdio.h>
#include <stdbool.h>


#include <yoyoengine/yoyoengine.h>

void editor_mkdir(const char *dir) {
    #ifdef _WIN32
        _mkdir(dir);
    #else
        mkdir(dir, 0777);
    #endif
    ye_logf(info, "EDITOR Created directory: %s\n", dir);
}

void editor_open_in_system(const char *url_or_file_path) {
    char command[512];

    #ifdef _WIN32
        snprintf(command, sizeof(command), "start %s", url_or_file_path);
    #elif defined(__APPLE__)
        snprintf(command, sizeof(command), "open %s", url_or_file_path);
    #else
        snprintf(command, sizeof(command), "xdg-open %s", url_or_file_path);
    #endif

    system(command);
}

void editor_touch_file(const char *file_path, const char *content) {
    // ensure the directory exists
    char *dir = strdup(file_path);
    char *last_slash = strrchr(dir, '/');
    if (last_slash) {
        *last_slash = '\0';
        editor_mkdir(dir);
    }

    FILE *file = fopen(file_path, "w");
    if (file) {
        if(content != NULL)
            fprintf(file, "%s", content);
        fclose(file);
        ye_logf(debug, "EDITOR Touched file: %s\n", file_path);
    }
    else {
        ye_logf(error, "EDITOR Failed to touch file: %s\n", file_path);
    }
}

bool editor_file_exists(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}