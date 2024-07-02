/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

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