/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <yoyoengine/ui/chart.h>

struct ye_chart *ye_new_chart(const char *name, int size) {
    struct ye_chart *chart = malloc(sizeof(struct ye_chart));
    chart->values = malloc(sizeof(struct ye_chart_value) * size);
    chart->name = strdup(name);
    chart->index = 0;
    chart->size = size;

    memset(chart->values, 0, sizeof(struct ye_chart_value) * size);

    return chart;
}

void ye_chart_push(struct ye_chart *chart, float value, int tick) {
    chart->values[chart->index].value = value;
    chart->values[chart->index].tick = tick;

    chart->index++;
    if(chart->index >= chart->size) {
        chart->index = 0;
    }
}

void ye_chart_destroy(struct ye_chart *chart) {
    free(chart->values);
    free(chart->name);
    free(chart);
}
