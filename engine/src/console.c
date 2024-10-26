/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/*
    TODO/IDEAS:
    - auto reccomendations (add a param for this too)
*/

#include <string.h>

#include <yoyoengine/engine.h>
#include <yoyoengine/console.h>
#include <yoyoengine/logging.h>

extern struct ye_console_command * cmd_head;

/*
    +----------+
    |  EXTERN  |
    +----------+
*/

struct ye_console_command * cmd_head = NULL;
struct ye_console_message * console_buffer = NULL;
int console_buffer_size = YE_DEFAULT_CONSOLE_BUFFER_SIZE;
int console_buffer_index = 0;
bool ye_console_color_code = true;
bool ye_console_reset_scroll = true;

/*
    +----------+
    | INTERNAL |
    +----------+
*/

void _clear_console_commands() {
    struct ye_console_command *current = cmd_head;
    while (current != NULL) {
        struct ye_console_command *next = current->next;
        
        if(current->prefix != NULL) {
            free(current->prefix);
            current->prefix = NULL;
        }

        free(current);
        current = next;
    }
    cmd_head = NULL;
}

void _register_default_commands() {
    // pass
}

/*
    +--------------------+
    | CONSTRUCT/TEARDOWN |
    +--------------------+
*/

void ye_init_console(int buffer_size) {
    /*
        Reset head
    */
    if(cmd_head != NULL) {
        _clear_console_commands();
        ye_logf(YE_LL_WARNING, "Weird edge case within the console!\n");
    }
    cmd_head = NULL;

    /*
        Console buffer setup
    */
    int console_buffer_size = buffer_size;
    if(console_buffer != NULL) {
        free(console_buffer);
        console_buffer = NULL;
    }
    console_buffer = (struct ye_console_message *)malloc(sizeof(struct ye_console_message) * console_buffer_size);
    console_buffer_index = 0;
    // initialize each message to null
    for(int i = 0; i < console_buffer_size; i++) {
        console_buffer[i].message_body = NULL;
        console_buffer[i]._cached_str = NULL;
    }

    /*
        Prefs / Flags
    */
    ye_console_color_code = true;
    ye_console_reset_scroll = true;

    /*
        Misc
    */
    _register_default_commands();

    ye_logf(YE_LL_INFO, "Initialized console.\n");
}

void _clear_console_buffer() {
    for(int i = 0; i < console_buffer_size; i++) {
        struct ye_console_message *current = &console_buffer[i];
        if(current->message_body != NULL) {
            free(current->message_body);
            current->message_body = NULL;
        }
        if(current->_cached_str != NULL) {
            free(current->_cached_str);
            current->_cached_str = NULL;
        }
    }
    console_buffer_index = 0;
}

void ye_shutdown_console() {
    _clear_console_buffer();

    _clear_console_commands();

    cmd_head = NULL;

    ye_logf(YE_LL_INFO, "Shut down console.\n");
}

/*
    +----------------+
    | VISUAL CONSOLE |
    +----------------+
*/

// index into this is YE_STATE.engine.log_level
// note: technically supress_all index is the same as reserved system ll
const char * current_log_level[] = {"debug+", "info+", "warning+", "error+", "supress all"};

void ye_paint_developer_console(struct nk_context * ctx) {
    if (nk_begin(ctx, "Console", nk_rect(300, 200, 1000, 633),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE)) {
    
        /*
            TOP BAR
        */
        nk_layout_row_dynamic(ctx, 30, 3);
        // color code
        nk_checkbox_label(ctx, "Color by log level", (nk_bool*)&ye_console_color_code);
        // Display threshold
        nk_label(ctx, "Threshold:", NK_TEXT_RIGHT);
        YE_STATE.engine.log_level = nk_combo(ctx, current_log_level, 5, YE_STATE.engine.log_level, 25, nk_vec2(200,200));

        /*
            LOG CONTENTS
        */
        // Calculate available space for log and input field
        float inputHeight = 35.0f; // Height of the input field
        float logHeight =  nk_window_get_height(ctx) - inputHeight - 100; // 100 is a random magic number offset that seems to work
        nk_layout_row_dynamic(ctx, logHeight, 1);
        nk_group_begin(ctx, "Log", NK_WINDOW_BORDER);
        for(int i = 0; i < console_buffer_size; i++) {
            int idx = (i + console_buffer_index) % console_buffer_size;
            struct ye_console_message *current = &console_buffer[idx];
            if(current->_cached_str == NULL) {
                continue;
            }

            nk_layout_row_dynamic(ctx, 15, 1);

            struct nk_color color = nk_rgb(255, 255, 255);  // white text
            if(ye_console_color_code) {
                switch(current->level){
                    case YE_LL_INFO:
                        // green
                        color = nk_rgb(0, 255, 0);
                        break;
                    case YE_LL_DEBUG:
                        // orange
                        color = nk_rgb(252, 186, 3);
                        break;
                    case YE_LL_WARNING:
                        // yellow
                        color = nk_rgb(248, 0, 252);
                        break;
                    case YE_LL_ERROR:
                        // red
                        color = nk_rgb(255, 0, 0);
                        break;
                    case _YE_RESERVED_LL_SYSTEM:
                        // light blue
                        color = nk_rgb(0, 255, 255);
                        break;
                    default:
                        break;
                }
            }
            // uhhh, for some reason nuklear is adding question marks to the end of our heap strings...
            char tmp[1024];
            snprintf(tmp, sizeof(tmp), "%s", current->_cached_str);
            // snprintf(tmp, sizeof(tmp), "%s", current->_cached_str);
            // tmp[20] = '\0';
            nk_label_colored(ctx, tmp, NK_TEXT_LEFT, color);

            // TODO: nk_label_colored_wrap
            nk_label_colored(ctx, (const char *)current->_cached_str, NK_TEXT_LEFT, color);

            // nk_label_colored(ctx, (const char *)current->_cached_str, NK_TEXT_LEFT, color);
            // nk_label_colored(ctx, (const char *)current->_cached_str, NK_TEXT_LEFT, color);
            
            // // debug for each character of the string, print it in printf as if it were an array into the terminal
            // for (int j = 0; current->_cached_str[j] != '\0'; j++) {
            //     printf("%c", current->_cached_str[j]);
            // }
            // printf("\n");

        }
        nk_group_end(ctx);

        /*
            COMMAND ENTRY
        */
        // scroll to bottom of group when console first opened
        if(should_reset_console_log_scroll){
            nk_group_set_scroll(YE_STATE.engine.ctx, "Log", 0, 10000);
            should_reset_console_log_scroll = false;
        }

        static char userInput[256] = {'\0'};
        nk_layout_row_dynamic(ctx, inputHeight, 1);
        
        // if we entered a command
        if (nk_input_is_key_pressed(&ctx->input, NK_KEY_ENTER)) {
            printf("enter pressed\n");
            
            if (strlen(userInput) > 0) {
                char newLine[sizeof("Console Command: ") + sizeof(userInput) + sizeof("\n") + 1];
                snprintf(newLine, sizeof(newLine), "Console Command: %s\n", userInput);

                ye_logf(_YE_RESERVED_LL_SYSTEM, newLine);

                ye_parse_console_command(userInput);

                memset(userInput, 0, sizeof(userInput));
            }
            // reset to bottom of log
            nk_group_set_scroll(ctx, "Log", 0, 10000);
        }

        // text field
        nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, userInput, sizeof(userInput), nk_filter_ascii);
    }
    nk_end(ctx);
}

/*
    +-------------------+
    | LOGGING / DISPLAY |
    +-------------------+
*/

const char * logLevelStrings[] = {"DEBUG", "INFO", "WARNING", "ERROR", "SYSTEM"};

void ye_console_push_message(const char * timestamp, enum logLevel level, const char * message_body) {
    /*
        Important: This function fires before the first setup
        (since we log before logging is intialized)

        Handle that: 
    */
    if(!console_buffer)
        // TODO: in the future, I want to permit such a queue push
        return;
    
    struct ye_console_message newMsg = {
        .level = level,
        .message_body = strdup(message_body)
    };
    snprintf(newMsg.timestamp, sizeof(newMsg.timestamp), "%s", timestamp);
    
    // free the command we are about to overwrite
    struct ye_console_message *current = &console_buffer[console_buffer_index % console_buffer_size];
    if(current->message_body != NULL) {
        free(current->message_body);
        current->message_body = NULL;
    }
    if(current->_cached_str != NULL) {
        free(current->_cached_str);
        current->_cached_str = NULL;
    }

    int llsz;
    char llstr[20];
    if(level == _YE_RESERVED_LL_SYSTEM){
        llsz = strlen("SYSTEM");
        snprintf(llstr, sizeof(llstr), "SYSTEM");
    }
    else{
        llsz = strlen(logLevelStrings[level]);
        snprintf(llstr, sizeof(llstr), "%s", logLevelStrings[level]);
    }
    // THIS SIZE NEEDS TO BE EXACT!
    int sz = strlen("[") + strlen(timestamp) + strlen("] [") + llsz + strlen("] ") + strlen(message_body);
    char * cached_str = (char *)malloc(sz);
    snprintf(cached_str, sz, "[%s] [%s] %s", timestamp, llstr, message_body);

    newMsg._cached_str = cached_str;

    // save new message
    console_buffer[console_buffer_index % console_buffer_size] = newMsg;

    console_buffer_index++;
}

void ye_parse_console_command(const char *command) {
    /*
        Extract the first contiguous block of text from the command,
        ex:
            "command arg1 arg2" -> "command"
            "command" -> "command"
    */
    char *prefix;   // one string of the prefix
    char **args;    // array of strings for the arguments
    int num_args;   // number of arguments (not counting prefix)

    // find the first space
    char *space = strchr(command, ' ');
    if(space == NULL) {
        prefix = strdup(command);
        args = NULL;
        num_args = 0;
    }
    else {
        /*
            TODO: segfaults
        */

        // calculate the length of the prefix
        int prefix_len = space - command + 1;
        prefix = (char *)malloc(prefix_len + 1);
        strncpy(prefix, command, prefix_len);
        prefix[prefix_len] = '\0';

        // calculate the number of arguments
        num_args = 0;
        for(char *c = space; *c != '\0'; c++) {
            if(*c == ' ') {
                num_args++;
            }
        }
        num_args++; // add one for the last argument

        // allocate space for the arguments
        args = (char **)malloc(sizeof(char *) * num_args);

        // copy the arguments
        char *arg_start = space + 1;
        for(int i = 0; i < num_args; i++) {
            char *arg_end = strchr(arg_start, ' ');
            if(arg_end == NULL) {
                args[i] = strdup(arg_start);
            }
            else {
                int arg_len = arg_end - arg_start;
                args[i] = (char *)malloc(arg_len + 1);
                strncpy(args[i], arg_start, arg_len);
                args[i][arg_len] = '\0';
            }
            arg_start = arg_end + 1;
        }
    }

    /*
        Check if a command is registered that accepts that prefix
    */
    bool fired = false;
    struct ye_console_command *current = cmd_head;
    while(current != NULL) {
        if(strcmp(current->prefix, prefix) == 0) {
            current->callback(num_args, (const char**)args);
            fired = true;
            break;
        }
        current = current->next;
    }

    if(!fired) {
        ye_logf(_YE_RESERVED_LL_SYSTEM, "Command not found: %s\n", prefix);
    }
}