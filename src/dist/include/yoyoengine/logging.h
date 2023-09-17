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

void ye_log_init(char *log_file_path);

void ye_log_shutdown();

void ye_logf(enum logLevel level, const char *format, ...);

void ye_paint_console(struct nk_context *ctx);

#endif