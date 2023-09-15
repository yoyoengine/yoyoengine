#ifndef UI_H
#define UI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <time.h>

#include <SDL2/SDL.h>

void init_ui(SDL_Window *win, SDL_Renderer *renderer);

void ui_handle_input(SDL_Event *evt);

void ui_begin_input_checks();

void ui_end_input_checks();

void ui_paint_debug_overlay();

void ui_render();

void shutdown_ui();

#endif