/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <yoyoengine/version.h>
#include <yoyoengine/logging.h>

void ye_get_version(const char *version_string, int *major, int *minor) {
    *major = 0;
    *minor = 0;

    if (sscanf(version_string, "build %d.%d", major, minor) == 2) {
        ye_logf(debug, "Parsed version: %d.%d\n", *major, *minor);
    }
    else if (sscanf(version_string, "build %d", major) == 1) {
        ye_logf(debug, "Parsed version: %d\n", *major);
    }
    else {
        ye_logf(error, "Failed to parse version string: %s\n", version_string);
        *major = -1;
        *minor = -1;
    }
    return;
}

bool ye_is_version_greater(const char *source, const char *target) {
    int source_major, source_minor, target_major, target_minor;
    ye_get_version(source, &source_major, &source_minor);
    ye_get_version(target, &target_major, &target_minor);

    if (source_major > target_major) {
        return true;
    }
    else if (source_major == target_major && source_minor > target_minor) {
        return true;
    }

    ye_logf(error, "Something really weird happened when comparing versions: %s -> %s\n", source, target);
    return false;
}

void ye_version_tagify(char *out) {
    // walk the version and replace spaces with hypens
    for (int i = 0; i < (int)strlen(out); i++) {
        if (out[i] == ' ') {
            out[i] = '-';
        }
    }
    out[strlen(out)] = '\0';
    return;
}