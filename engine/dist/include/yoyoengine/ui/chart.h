/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YE_CHART_H
#define YE_CHART_H

#include <yoyoengine/export.h>

struct ye_chart_value {
    float value;
    int tick;
};

struct ye_chart {
    struct ye_chart_value *values;
    char *name;
    int index;
    int size;

    // TODO: later idk. can have it handle drawing too
    // int tick_delay_ms;
    
    // "private"
    // int _last_tick;
};

YE_API struct ye_chart *ye_new_chart(const char *name, int size);

YE_API void ye_chart_push(struct ye_chart *chart, float value, int tick);

YE_API void ye_chart_destroy(struct ye_chart *chart);

#endif // YE_CHART_H