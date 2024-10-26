/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <yoyoengine/commands.h>
#include <yoyoengine/console.h>

void ye_cmd_help(int argc, const char **argv) {
    if(argc < 0) {
        ye_logf(_YE_RESERVED_LL_SYSTEM, "Why are you asking for help on the help command??\n");
        ye_logf(_YE_RESERVED_LL_SYSTEM, "Epic easter egg moment!\n");
        return;
    }

    /*
        Convention for [help]
    */
    if(argc < 1) {
        ye_logf(_YE_RESERVED_LL_SYSTEM, "Usage: help (optional)[command prefix]\n");
        ye_logf(_YE_RESERVED_LL_SYSTEM, "   [command prefix]: the name of a command you wish to learn more about.\n");
        ye_logf(_YE_RESERVED_LL_SYSTEM, "                     (warning): it's up to the command developer to\n");
        ye_logf(_YE_RESERVED_LL_SYSTEM, "                     implement the [help] functionality. This might not\n");
        ye_logf(_YE_RESERVED_LL_SYSTEM, "                     work properly for all commands!\n");
        // return;
    }

    // ye_logf(_YE_RESERVED_LL_SYSTEM, "\n");

    if(argc <= 0) {
        ye_logf(_YE_RESERVED_LL_SYSTEM, "Available commands:\n");

        struct ye_console_command *current = cmd_head;
        while(current != NULL) {
            ye_logf(_YE_RESERVED_LL_SYSTEM, "    %s\n", current->prefix);
            current = current->next;
        }

        return;
    }

    if(argc > 1) {
        ye_logf(_YE_RESERVED_LL_SYSTEM, "Warning! you have entered %d arguments to help, which accepts 1 argument at maximum. argc>1 will be ignored.\n",argc);
    }
    
    // try to fire the desired command in a "help" state (argc = -1)
    bool res = ye_execute_console_command(argv[0], -1, NULL);
    if(!res) {
        ye_logf(_YE_RESERVED_LL_SYSTEM, "Could not run help on command: \"%s\" (not found)\n", argv[0]);
    }
}