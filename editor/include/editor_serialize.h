/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef EDITOR_SERIALIZE_H
#define EDITOR_SERIALIZE_H

#include "editor.h"
#include <yoyoengine/yoyoengine.h>

/*
    Take the currently opened scene and write it to disk at the provided path
*/
void editor_write_scene_to_disk(const char *path);

#endif