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

#include <time.h>
#include <stdio.h>
#include <stdarg.h>

#include <yoyoengine/yoyoengine.h>

#ifdef _WIN32
#include <windows.h>
#endif

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
        printf("%sError opening logfile at: %s\n",logpath,RED);
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

/*
    Log a message to the console and log file
    CAN ONLY BE USED AFTER INITIALIZING ENGINE

    TODO: it would be cool to have
    - option to log only to file or console
    - not reliant on SDL and could be used whenever
*/
void ye_logf(enum logLevel level, const char *format, ...){
    // if logging is disabled, or the log level is below the threshold, return (or if the file is not open yet)
    if(engine_state.log_level > level || logFile == 0x0){ // idk why i wrote null like this i just want to feel cool
        return;
    }

    // prepare our formatted string
    va_list args;
    va_start(args, format);

    char text[1024]; // Adjust the buffer size as needed
    vsnprintf(text, sizeof(text), format, args);

    va_end(args);
    
    // printf("%s",text);
    // return;

    switch (level) {
        case debug:
            fprintf(logFile, "[%s] [DEBUG]: %s", ye_get_timestamp(), text);
            printf("%s[%s] [%sDEBUG%s]: %s", RESET, ye_get_timestamp(), MAGENTA, RESET, text);
            break;
        case info:
            fprintf(logFile, "[%s] [INFO]:  %s", ye_get_timestamp(), text);
            printf("%s[%s] [%sINFO%s]:  %s", RESET, ye_get_timestamp(), GREEN, RESET, text);
            break;
        case warning:
            fprintf(logFile, "[%s] [WARNING]: %s", ye_get_timestamp(), text);
            printf("%s[%s] [%sWARNING%s]: %s", YELLOW, ye_get_timestamp(), YELLOW, RESET, text);
            break;
        case error:
            fprintf(logFile, "[%s] [ERROR]: %s", ye_get_timestamp(), text);
            printf("%s[%s] [%sERROR%s]: %s", RED, ye_get_timestamp(), RED, RESET, text);
            break;
        default:
            fprintf(logFile, "[%s] [ERROR]: %s", ye_get_timestamp(), "Invalid log level\n");
            printf("%s[%s] [%sERROR%s]: %s", RED, ye_get_timestamp(), RED, RESET, "Invalid log level\n");
            break;
    }
    engine_runtime_state.log_line_count++;

    // Add to the log buffer
    ye_add_to_log_buffer(level, text);
}

void ye_log_newline(enum logLevel level){
    // if logging is disabled, or the log level is below the threshold, return
    if(engine_state.log_level > level){
        return;
    }
    fprintf(logFile, "\n");
    printf("\n");
}


// buffer and console ui related ops

#define LOG_BUFFER_SIZE 100

struct LogMessage {
    char timestamp[20];
    enum logLevel level;
    char text[256]; // Adjust the size as needed
};

struct LogMessage logBuffer[LOG_BUFFER_SIZE];
int logBufferIndex = 0;

/*
    shifts old messages out of buffer
*/
void ye_add_to_log_buffer(enum logLevel level, const char *text) {
    // Initialize a new message
    struct LogMessage message;
    snprintf(message.timestamp, sizeof(message.timestamp), "%s", ye_get_timestamp());
    message.level = level;

    // Remove newline characters from the text
    size_t text_length = strnlen(text, sizeof(message.text));
    if (text_length > 0 && text[text_length - 1] == '\n') {
        text_length--; // Remove the trailing newline
    }
    snprintf(message.text, sizeof(message.text), "%.*s", (int)text_length, text);

    // If the buffer is full, shift messages to make space for the new one
    if (logBufferIndex == LOG_BUFFER_SIZE) {
        // Shift all messages except the first one
        for (int i = 1; i < LOG_BUFFER_SIZE; i++) {
            logBuffer[i - 1] = logBuffer[i];
        }
        logBufferIndex--; // Decrease the index to overwrite the last message
    }

    // Add the new message to the buffer
    logBuffer[logBufferIndex] = message;
    logBufferIndex++;
}

#define MAX_INPUT_LENGTH 100
char userInput[MAX_INPUT_LENGTH];

bool color_code = true;

/*
    Paint log console

    in the future to save window bounds:
        nk_window_get_bounds(ctx);
    
    in the future it would also be cool to toggle the log level threshold

    TODO:
    - with advent of the editor I would like for this to get moved out of here or at least the 
      command logic so that its something the editor has control over, 
      and can provide a visual interface for most commands
*/
void ye_paint_console(struct nk_context *ctx){
    // Create the GUI layout
    if (nk_begin(ctx, "Console", nk_rect(300, 200, 1000, 633),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)) {
        
        nk_layout_row_dynamic(ctx, 30, 3);
        nk_checkbox_label(ctx, "Color by log level", &color_code);
        
        nk_label(ctx, "Threshold:", NK_TEXT_RIGHT);
        switch(engine_state.log_level){
            case 0:
                nk_text_colored(ctx, "debug+", 6, NK_TEXT_LEFT, nk_rgb(252, 186, 3));  // Green text
                break;
            case 1:
                nk_text_colored(ctx, "info+", 5, NK_TEXT_LEFT, nk_rgb(0, 255, 0));  // Orange text
                break;
            case 2:
                nk_text_colored(ctx, "warning+", 8, NK_TEXT_LEFT, nk_rgb(248, 0, 252));  // Yellow text
                break;
            case 3:
                nk_text_colored(ctx, "error+", 6, NK_TEXT_LEFT, nk_rgb(255, 0, 0));  // Red text
                break;
            default:
                nk_text_colored(ctx, "supress all", 11, NK_TEXT_LEFT, nk_rgb(255, 255, 255));  // white text
                break;
        }

        // Calculate available space for log and input field
        float inputHeight = 35.0f; // Height of the input field
        float logHeight =  500.0f; // ctx->current->layout->row.height - inputHeight;

        // TODO: allow resizing again and auto calculate size of log panel
        // printf("height: %f\n",ctx->current->bounds.h);

        // struct nk_scroll horizontal_scroll; TODO add horizontal scrolling
        // nk_zero(&horizontal_scroll, sizeof(struct nk_scroll));

        nk_layout_row_dynamic(ctx, logHeight, 1);
        nk_group_begin(ctx, "Log", NK_WINDOW_BORDER);

        for (int i = 0; i < logBufferIndex; i++) {
            nk_layout_row_dynamic(ctx, 15, 1);

            // Create a formatted log entry with timestamp and color
            char formattedLog[256];
            snprintf(formattedLog, sizeof(formattedLog), "<%s> %s", logBuffer[i].timestamp, logBuffer[i].text);

            // Display the formatted log entry
            if(color_code){
                switch(logBuffer[i].level){
                    case info:
                        nk_label_colored(ctx, formattedLog, NK_TEXT_LEFT, nk_rgb(0, 255, 0));  // Green text
                        break;
                    case debug:
                        nk_label_colored(ctx, formattedLog, NK_TEXT_LEFT, nk_rgb(252, 186, 3));  // Orange text
                        break;
                    case warning:
                        nk_label_colored(ctx, formattedLog, NK_TEXT_LEFT, nk_rgb(248, 0, 252));  // Yellow text
                        break;
                    case error:
                        nk_label_colored(ctx, formattedLog, NK_TEXT_LEFT, nk_rgb(255, 0, 0));  // Red text
                        break;
                    default:
                        break;
                }
            }
            else{
                nk_label_colored(ctx, formattedLog, NK_TEXT_LEFT, nk_rgb(255, 255, 255));  // white text
            }
        }

        nk_group_end(ctx);
        // Input command section
        static char userInput[MAX_INPUT_LENGTH];
        nk_layout_row_dynamic(ctx, inputHeight, 1);
        if (nk_input_is_key_pressed(&ctx->input, NK_KEY_ENTER)) {
            // Handle user input when Enter key is pressed
            if (strlen(userInput) > 0) {
                // log the executed command TODO: add \n here
                ye_logf(debug, userInput);
                ye_log_newline(debug);

                // TODO: i would love to move command execution to a more sensible location in the future
                if(strcmp(userInput,"entlist")==0){
                    ye_print_entities();
                }
                else if(strcmp(userInput,"help")==0){
                    ye_logf(debug,"Available commands: entlist, toggle paintbounds, toggle freecam, reload scene\n");
                }
                else if(strcmp(userInput,"reload scene")==0){
                    ye_reload_scene();
                }
                // check if the first word (there can be words after seperated by spaces) is "toggle"
                else if(strncmp(userInput,"toggle",6)==0){
                    // check if the second word is "debug"
                    if(strncmp(userInput+7,"paintbounds",11)==0){
                        engine_state.paintbounds_visible = !engine_state.paintbounds_visible;
                    }
                    else if(strncmp(userInput+7,"freecam",7)==0){
                        engine_state.freecam_enabled = !engine_state.freecam_enabled;
                        if(engine_state.freecam_enabled){
                            ye_logf(debug,"Freecam enabled\n");
                        }
                        else{
                            ye_logf(debug,"Freecam disabled\n");
                        }
                    }
                    else{
                        ye_logf(debug,"Unknown toggle command. Type 'help' for a list of commands\n");
                    }
                }
                else{
                    ye_logf(debug,"Unknown command. Type 'help' for a list of commands\n");
                }
                
                // Clear the input buffer
                memset(userInput, 0, sizeof(userInput));
            }
        }
        nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, userInput, sizeof(userInput), nk_filter_ascii);
        // nk_edit_focus(ctx, NK_EDIT_FIELD); TODO: really wish i could focus this auto
        // https://github.com/vurtun/nuklear/issues/516
    }
    nk_end(ctx);
}

// INIT AND SHUTDOWN ////////////////////////

void ye_log_init(char * log_file_path){
    logpath = log_file_path;

    // windows specific tweak to enable ansi colors
    #ifdef _WIN32
    enableVirtualTerminal();
    #endif

    // open log file the first time in w mode to overwrite any existing log
    if(engine_state.log_level < 4){
        ye_open_log();
        ye_logf(info, "Logging initialized\n");
        engine_runtime_state.log_line_count=1; // reset our counter because not all outputs have actually been written to the log file yet
    }
}

void ye_log_shutdown(){
    ye_logf(info, "Logging shutdown\n");
    engine_runtime_state.log_line_count++;
    ye_close_log();
}