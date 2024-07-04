/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file logging.h
 * @brief The engine Logging API
 */

#ifndef LOGGING_H
#define LOGGING_H

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

#include <stdbool.h>

extern bool should_reset_console_log_scroll;

/**
 * @brief Describes the level of a log message
 */
enum logLevel {
    debug,
    info,
    warning,
    error
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

/**
 * @brief UI function to paint a visual console
 * 
 * @param ctx The Nuklear context
 */
void ye_paint_console(struct nk_context *ctx);

#endif