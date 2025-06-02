/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YE_EXPORT_H
#define YE_EXPORT_H

// #ifdef _WIN32
//     // if we are building, export, else import
//     #ifdef YOYO_ENGINE_BUILDING
//         #define YE_API __declspec(dllexport)
//     #else
//         #define YE_API __declspec(dllimport)
//     #endif
// #else
#define YE_API
// #endif

#endif