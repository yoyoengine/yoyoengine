#ifndef LOGGING_H
#define LOGGING_H

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

#endif