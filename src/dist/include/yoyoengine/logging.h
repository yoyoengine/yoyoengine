#ifndef LOGGING_H
#define LOGGING_H

#ifndef nk
#define nk
#include <Nuklear/nuklear.h>
#endif

enum logLevel {
    debug,
    info,
    warning,
    error
};

void log_init();

void log_shutdown();

void logMessage(enum logLevel level, const char *text);

void paint_console(struct nk_context *ctx);

#endif