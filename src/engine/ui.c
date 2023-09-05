#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <time.h>

#include <yoyoengine/yoyoengine.h>

#include <SDL2/SDL.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION

#include <Nuklear/nuklear.h>
#include <Nuklear/nuklear_sdl_renderer.h>

float font_scale = 1;

struct nk_context *ctx;

void init_ui(SDL_Window *win, SDL_Renderer *renderer){
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    /* scale the renderer output for High-DPI displays */
    {
        int render_w, render_h;
        int window_w, window_h;
        float scale_x, scale_y;
        SDL_GetRendererOutputSize(renderer, &render_w, &render_h);
        SDL_GetWindowSize(win, &window_w, &window_h);
        scale_x = (float)(render_w) / (float)(window_w);
        scale_y = (float)(render_h) / (float)(window_h);
        SDL_RenderSetScale(renderer, scale_x, scale_y);
        font_scale = scale_y;
    }
    ctx = nk_sdl_init(win, renderer);
    /* GUI */
    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    {
        struct nk_font_atlas *atlas;
        struct nk_font_config config = nk_font_config(0);
        struct nk_font *font;

        /* set up the font atlas and add desired font; note that font sizes are
         * multiplied by font_scale to produce better results at higher DPIs */
        nk_sdl_font_stash_begin(&atlas);
        font = nk_font_atlas_add_from_file(atlas, getEngineResourceStatic("Orbitron-Regular.ttf"), 20 * font_scale, &config);
        nk_sdl_font_stash_end();

        /* this hack makes the font appear to be scaled down to the desired
         * size and is only necessary when font_scale > 1 */
        font->handle.height /= font_scale;
        /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
        nk_style_set_font(ctx, &font->handle);
    }
    logMessage(info, "ui initialized");
}

void ui_handle_input(SDL_Event *evt){
    nk_sdl_handle_event(evt);
}

void ui_begin_input_checks(){
    nk_input_begin(ctx);
}

void ui_end_input_checks(){
    nk_input_end(ctx);
}

/*
    TODO: goal is to abstract painting ui overlay objects.

    Considerations:
    - are we letting the programmer create their own gui overlays?
        - this depends on if this will be used for text field inputs and other inputs, or just dev overlays
    - do we need to make this extensible to lua? there was some talk about lua bindings on the Nuklear Repo
    - For multiple overlays at the same time and visible/invisible overlay states, we need to collect function pointers that paint overlays
      and throw them into a queue this ui file can iterate through to paint them all.
    - relative positioning translation for ui comps... also how will we arrange viewport for game in editor

    Features:
    - would be nice to get a perf/benchmarking/timing suite in the codebase so we can see all timing elements of a frame
      here, like how long logic/input/ui paint/object paint/interact check/physics took
*/
void ui_paint_debug_overlay(int fps, int paint_time, int render_object_count, int audio_chunk_count, int log_line_count){
    // put all the parameters into strings for display
    char fps_str[100];
    char paint_time_str[100];
    char render_object_count_str[100];
    char audio_chunk_count_str[100];
    char log_line_count_str[100];
    sprintf(fps_str, "fps: %d", fps);
    sprintf(paint_time_str, "paint time: %dms", paint_time);
    sprintf(render_object_count_str, "render object count: %d", render_object_count);
    sprintf(audio_chunk_count_str, "audio chunk count: %d", audio_chunk_count);
    sprintf(log_line_count_str, "log line count: %d", log_line_count);
    
    if (nk_begin(ctx, "Metrics", nk_rect(10, 10, 220, 200),
                    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)) {
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_label(ctx, fps_str, NK_TEXT_LEFT);
        nk_label(ctx, paint_time_str, NK_TEXT_LEFT);
        nk_label(ctx, render_object_count_str, NK_TEXT_LEFT);
        nk_label(ctx, audio_chunk_count_str, NK_TEXT_LEFT);
        nk_label(ctx, log_line_count_str, NK_TEXT_LEFT);
    }
    nk_end(ctx);
}

void ui_render(){
    nk_sdl_render(NK_ANTI_ALIASING_ON);
}

void shutdown_ui(){
    nk_sdl_shutdown();
    logMessage(info, "Shut down UI\n");
}