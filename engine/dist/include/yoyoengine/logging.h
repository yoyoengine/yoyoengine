/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file logging.h
 * @brief The engine Logging API
 */

#ifndef LOGGING_H
#define LOGGING_H

#ifndef __cplusplus
#ifndef ye_nk
#define ye_nk

/*
    On windows, we need nk fixed types to avoid errors...
    not sure why but it originated in theriac era
*/
#ifdef _WIN32
    #define NK_INCLUDE_FIXED_TYPES
#endif

#include <Nuklear/nuklear.h>
#endif
#endif

#include <stdbool.h>

/**
 * @brief Describes the level of a log message
 */
enum logLevel {
    /*
        DEPRECATED: v1 naming convention
    */
    debug,
    info,
    warning,
    error,

    /*
        v2 naming convention (more explicit)

        Should be used going forwards, but old
        enums will remain for compatibility
    */
    YE_LL_DEBUG = debug,
    YE_LL_INFO = info,
    YE_LL_WARNING = warning,
    YE_LL_ERROR = error,

    /*
        RESERVED: for internal use
    */
    _YE_RESERVED_LL_SYSTEM = -1, // used to denote call/response from dev console
    // ^^ -1 so we can keep > error as NONE
};

/**
 * @brief Initializes the logging system
 * 
 * @param log_file_path The path to the log file
 */
void ye_log_init(char *log_file_path);

/**
 * @brief Shuts down the logging system
 */
void ye_log_shutdown();

/**
 * @brief Logs a message to the console and console buffer
 * 
 * @param level The level of the message
 * @param format The content of the message (similar to printf)
 * @param ... The arguments for the format string
 */
void ye_logf(enum logLevel level, const char *format, ...);

/**
 * @brief THIS IS FOR INTERNAL USE ONLY. Logs a message normally but with a lua tag in front of the output.
 * 
 * @param level The level of the message
 * @param format The content of the message (similar to printf)
 * @param ... The arguments for the format string
 */
void _ye_lua_logf(enum logLevel level, const char *format, ...);

#endif