/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YE_EDITOR_BUILD_H
#define YE_EDITOR_BUILD_H

#include <stdbool.h>

void editor_build_packs(bool force);

void editor_build();

void editor_build_and_run();

void editor_run();

void editor_build_reconfigure();

#endif