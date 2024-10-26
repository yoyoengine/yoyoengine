#include <yoyoengine/commands.h>
#include <yoyoengine/console.h>

void ye_cmd_help(int argc, const char **argv) {
    // mitigate unused parameter warning
    (void)argc;
    (void)argv;

    ye_logf(_YE_RESERVED_LL_SYSTEM, "Available commands:\n");

    struct ye_console_command *current = cmd_head;
    while(current != NULL) {
        ye_logf(_YE_RESERVED_LL_SYSTEM, "    %s\n", current->prefix);
        current = current->next;
    }
}