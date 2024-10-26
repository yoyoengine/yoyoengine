/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
    @file console.h
    @brief Provides definitions and API for the developer console.
*/

#ifndef CONSOLE_H
#define CONSOLE_H

#include <yoyoengine/logging.h>

#include <Nuklear/nuklear.h>

#include <stdlib.h>
#include <stdbool.h>

/**
 * @brief Paint function to create the developer console in Nuklear
 * 
 * @param ctx The nk_context *
 */
void ye_paint_developer_console(struct nk_context *ctx);

/**
 * @brief Register a console command to be fired by the console
 * 
 * @param prefix The first full keyterm to match against your command
 * @param callback A void(*)(const char *) function to be called when the command is matched, passed the rest of the string body
 */
void ye_register_console_command(const char *prefix, void (*callback)(int, const char **));

/**
 * @brief Parse a string and execute it as a command 
 * 
 * Will implicitly fire the appropriate callbacks as needed
 * 
 * @param command The string to parse
 */
void ye_parse_console_command(const char *command);

/*
    Definition of a console command,
    linked list
*/
struct ye_console_command {
    char *prefix;
    // int numargs, const char ** args
    void (*callback)(int, const char **);

    struct ye_console_command *next;
};

extern struct ye_console_command * cmd_head;

/*
    Setup and validate data for the console
*/
void ye_init_console();

/*
    Clear data and destroy the console
*/
void ye_shutdown_console();

/*
    Definitions for console buffer
*/

struct ye_console_message {
    char timestamp[20];
    enum logLevel level;    // _YE_LL_RESERVED_SYSTEM is used for non logging messages
    char * message_body;

    char * _cached_str;
};

/*
    Resizable array of console messages
*/
#ifndef YE_DEFAULT_CONSOLE_BUFFER_SIZE
    #define YE_DEFAULT_CONSOLE_BUFFER_SIZE 100
#endif
extern struct ye_console_message * console_buffer;
extern int console_buffer_size;
extern int console_buffer_index;


/*
    Preferences / Flags
*/
extern bool ye_console_color_code;
extern bool ye_console_reset_scroll;

/*
    Submitting messages
*/

/**
 * @brief Push a message to the console buffer
 * 
 * @param timestamp The timestamp of the message (do not malloc this)
 * @param level The level of the message
 * @param message_body The body of the message (do not malloc this)
 */
void ye_console_push_message(const char * timestamp, enum logLevel level, const char * message_body);

#endif