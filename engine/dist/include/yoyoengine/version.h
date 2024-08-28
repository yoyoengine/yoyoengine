/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef VERSION_H
#define VERSION_H

// minor version should go largely unused, but i'm reserving it just in case
#define YOYO_ENGINE_MAJOR_VERSION   0
#define YOYO_ENGINE_MINOR_VERSION   0

#define YOYO_ENGINE_VERSION_STRING  "build 0"

// versions of other systems and file formats within the engine
#define YOYO_ENGINE_SCENE_VERSION           0 // version 0 of scene files
#define YOYO_ENGINE_STYLES_VERSION          0 // version 0 of style files
#define YOYO_ENGINE_ANIMATION_FILE_VERSION  0 // version 0 of animation files

#include <stdbool.h>

void ye_get_version(const char *version_string, int *major, int *minor);

bool ye_is_version_greater(const char *source, const char *target);

void ye_version_tagify(const char *version_string, char *out);

#endif