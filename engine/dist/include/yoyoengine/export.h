/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YE_EXPORT_H
#define YE_EXPORT_H

#ifdef _WIN32
    #define YE_API __declspec(dllexport)
#else
    #define YE_API
#endif

#endif