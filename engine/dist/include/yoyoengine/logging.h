/*
    This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/**
 * @file logging.h
 * @brief The engine Logging API
 */

#ifndef LOGGING_H
#define LOGGING_H

#ifndef nk
#define nk
#include <Nuklear/nuklear.h>
#endif

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