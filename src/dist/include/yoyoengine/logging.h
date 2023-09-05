#ifndef LOGGING_H
#define LOGGING_H

#include <Nuklear/nuklear.h> // struct nk_context

extern int linesWritten;

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