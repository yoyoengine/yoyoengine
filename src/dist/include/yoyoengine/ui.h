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

void ui_paint_debug_overlay(int fps, int paint_time, int render_object_count, int audio_chunk_count, int log_line_count);

void ui_render();

void shutdown_ui();