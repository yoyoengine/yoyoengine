/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

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

#ifdef _WIN32
    #include <windows.h>
#endif
#endif

#include <yoyoengine/scene.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/console.h>
#include <yoyoengine/logging.h>
#include <yoyoengine/ecs/ecs.h>

// ANSI escape codes for color.
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

FILE *logFile = NULL;
char *logpath = NULL;

#ifdef _WIN32
void ye_enable_virtual_terminal() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;

    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#endif

const char* ye_get_timestamp() {
    static char datetime_str[20];
    time_t now = time(NULL);
    struct tm local_time = *localtime(&now);

    strftime(datetime_str, sizeof(datetime_str), "%Y-%m-%d %H:%M:%S", &local_time);

    return datetime_str;
}

void ye_open_log(){
    // open the log file
    logFile = fopen(logpath, "w");
    if (logFile == NULL) {
        printf("%sError opening logfile at: %s%s\n",RED,logpath,RESET);
    }
    else{
        fseek(logFile, 0, SEEK_END);
    }
}

void ye_close_log(){
    if(logFile != 0x0){
        fclose(logFile);
    }
}

void ye_logf(enum logLevel level, const char *format, ...){
    /*
        Format from varargs
    */
    va_list args;
    va_start(args, format);
    char text[1024];
    vsnprintf(text, sizeof(text), format, args);
    va_end(args);
    
    if(level == warning)
        YE_STATE.runtime.warning_count++;
    else if(level == error)
        YE_STATE.runtime.error_count++;

    // ALWAYS push to dev console buffer
    // TODO: subject to change for optimization later
    ye_console_push_message(ye_get_timestamp(), level, text);

    // if logging is disabled, or the log level is below the threshold, return (or if the file is not open yet)
    if((enum logLevel)YE_STATE.engine.log_level > level && level != _YE_RESERVED_LL_SYSTEM)
        return;

    // if logfile unititialized, put it in the buffer anyways (because it meets threshold), and if we are in debug mode then print to stdout as well
    if(logFile == 0x0){
        // if we are in debug mode put it in stdout
        if(YE_STATE.engine.debug_mode){
            printf("%s",text);
        }
        return;
    }

    switch (level) {
        case YE_LL_DEBUG:
            fprintf(logFile, "[%s] [DEBUG]: %s", ye_get_timestamp(), text);
            printf("%s[%s] [%sDEBUG%s]: %s", RESET, ye_get_timestamp(), MAGENTA, RESET, text);
            break;
        case YE_LL_INFO:
            fprintf(logFile, "[%s] [INFO]:  %s", ye_get_timestamp(), text);
            printf("%s[%s] [%sINFO%s]:  %s", RESET, ye_get_timestamp(), GREEN, RESET, text);
            break;
        case YE_LL_WARNING:
            fprintf(logFile, "[%s] [WARNING]: %s", ye_get_timestamp(), text);
            printf("%s[%s] [WARNING]%s: %s", YELLOW, ye_get_timestamp(), RESET, text);
            break;
        case YE_LL_ERROR:
            fprintf(logFile, "[%s] [ERROR]: %s", ye_get_timestamp(), text);
            printf("%s[%s] [ERROR]%s: %s", RED, ye_get_timestamp(), RESET, text);
            break;
        case _YE_RESERVED_LL_SYSTEM:
            fprintf(logFile, "[%s] [SYSTEM]: %s", ye_get_timestamp(), text);
            printf("%s[%s] [SYSTEM]%s: %s", CYAN, ye_get_timestamp(), RESET, text);
            break;
        default:
            fprintf(logFile, "[%s] [ERROR]: %s", ye_get_timestamp(), "Invalid log level\n");
            printf("%s[%s] [LOG ERROR]%s: %s", RED, ye_get_timestamp(), RESET, "Invalid log level\n");
            break;
    }
    YE_STATE.runtime.log_line_count++;
}

/*
    TODO: this is stupidly just a ctrl-c ctrl-v of the above function, because we slightly change
    the formatted output. FIX THIS!
*/
void _ye_lua_logf(enum logLevel level, const char *format, ...){
    /*
        Format from varargs
    */
    va_list args;
    va_start(args, format);
    char text[1024];
    vsnprintf(text, sizeof(text), format, args);
    va_end(args);
    
    if(level == warning)
        YE_STATE.runtime.warning_count++;
    else if(level == error)
        YE_STATE.runtime.error_count++;

    // ALWAYS push to dev console buffer
    // TODO: subject to change for optimization later
    ye_console_push_message(ye_get_timestamp(), level, text);

    // if logging is disabled, or the log level is below the threshold, return (or if the file is not open yet)
    if((enum logLevel)YE_STATE.engine.log_level > level && level != _YE_RESERVED_LL_SYSTEM)
        return;

    // if logfile unititialized, put it in the buffer anyways (because it meets threshold), and if we are in debug mode then print to stdout as well
    if(logFile == 0x0){
        // if we are in debug mode put it in stdout
        if(YE_STATE.engine.debug_mode){
            printf("%s",text);
        }
        return;
    }

    switch (level) {
        case YE_LL_DEBUG:
            fprintf(logFile, "[%s] [LUA] [DEBUG]: %s", ye_get_timestamp(), text);
            printf("%s[%s] [%sLUA%s] [%sDEBUG%s]: %s", RESET, ye_get_timestamp(), BLUE, RESET, MAGENTA, RESET, text);
            break;
        case YE_LL_INFO:
            fprintf(logFile, "[%s] [LUA] [INFO]:  %s", ye_get_timestamp(), text);
            printf("%s[%s] [%sLUA%s] [%sINFO%s]:  %s", RESET, ye_get_timestamp(), BLUE, RESET, GREEN, RESET, text);
            break;
        case YE_LL_WARNING:
            fprintf(logFile, "[%s] [LUA] [WARNING]: %s", ye_get_timestamp(), text);
            printf("%s[%s] [%sLUA%s] [%sWARNING%s]: %s", RESET, ye_get_timestamp(), BLUE, RESET, YELLOW, RESET, text);
            break;
        case YE_LL_ERROR:
            fprintf(logFile, "[%s] [LUA] [ERROR]: %s", ye_get_timestamp(), text);
            printf("%s[%s] [%sLUA%s] [%sERROR%s]: %s", RESET, ye_get_timestamp(), BLUE, RESET, RED, RESET, text);
            break;
        case _YE_RESERVED_LL_SYSTEM:
            fprintf(logFile, "[%s] [LUA] [SYSTEM]: %s", ye_get_timestamp(), text);
            printf("%s[%s] [%sLUA%s] [%sSYSTEM%s]: %s", RESET, ye_get_timestamp(), BLUE, RESET, CYAN, RESET, text);
            break;
        default:
            fprintf(logFile, "[%s] [ERROR]: %s", ye_get_timestamp(), "Invalid log level\n");
            printf("%s[%s] [%sLUA%s] [%sERROR%s]: %s", RESET, ye_get_timestamp(), BLUE, RESET, RED, RESET, "Invalid log level\n");
            break;
    }
    YE_STATE.runtime.log_line_count++;
}

void ye_log_init(char * log_file_path){
    logpath = log_file_path;

    // windows specific tweak to enable ansi colors
    #ifdef _WIN32
    ye_enable_virtual_terminal();
    #endif

    // open log file the first time in w mode to overwrite any existing log
    if(YE_STATE.engine.log_level < 4){
        ye_open_log();
        ye_logf(info, "Initialized logging.\n");
        YE_STATE.runtime.log_line_count=1; // reset our counter because not all outputs have actually been written to the log file yet
    }
}

void ye_log_shutdown(){
    ye_logf(info, "Shut down logging.\n");
    YE_STATE.runtime.log_line_count++;
    ye_close_log();
}