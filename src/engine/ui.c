/*
    This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION

#ifndef nk
#define nk
#include <Nuklear/nuklear.h>
#endif

#include <Nuklear/nuklear_sdl_renderer.h>
#include <Nuklear/style.h>

#include <yoyoengine/yoyoengine.h>

#define MAX_UI_COMPONENTS 30
#define MAX_KEY_LENGTH 100

float font_scale = 1;

struct nk_context *ctx;

////////////////////////////////

typedef struct {
    char key[MAX_KEY_LENGTH];  // Use a key identifier
    void (*render_function)(struct nk_context *ctx);
    // Add other relevant data fields here
} UIComponent;


UIComponent ui_components[MAX_UI_COMPONENTS];
int num_ui_components = 0;  // Track the number of registered components

void ui_register_component(const char* key, void (*render_function)()) {
    if (num_ui_components < MAX_UI_COMPONENTS) {
        UIComponent new_component;
        strncpy(new_component.key, key, MAX_KEY_LENGTH - 1);  // Copy the key

        new_component.render_function = render_function;
        // Add any other relevant initialization here

        ui_components[num_ui_components++] = new_component;
    } else {
        // Handle error: Max UI components reached
    }
}

void remove_ui_component(const char* key) {
    for (int i = 0; i < num_ui_components; i++) {
        UIComponent* component = &ui_components[i];
        if (strcmp(component->key, key) == 0) {
            // Found the matching component, remove it by shifting elements
            for (int j = i; j < num_ui_components - 1; j++) {
                ui_components[j] = ui_components[j + 1];
            }
            num_ui_components--;
            return;  // Component removed, exit the loop
        }
    }
}

////////////////////////////////

// void paint_test(struct nk_context *ctx){
//     if (nk_begin(ctx, "Test", nk_rect(10, 10, 220, 200),
//                     NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)) {
//         nk_layout_row_dynamic(ctx, 25, 1);
//         nk_label(ctx, "Hello World!", NK_TEXT_LEFT);
//     }
//     nk_end(ctx);
// }

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
    - how do we extend gui overlay creation to lua/python? would we be forcing C?
        - do we need to make this extensible to lua? there was some talk about lua bindings on the Nuklear Repo
    - integration for general game inputs and such, a default settings menu ships with game scenes?
    - relative positioning translation for ui comps... also how will we arrange viewport for game in editor

    Features:
    - would be nice to get a perf/benchmarking/timing suite in the codebase so we can see all timing elements of a frame
      here, like how long logic/input/ui paint/object paint/interact check/physics took
*/
void ui_paint_debug_overlay(){
    // put all the parameters into strings for display
    char fps_str[100];
    char paint_time_str[100];
    char entity_count_str[100];
    char audio_chunk_count_str[100];
    char log_line_count_str[100];
    sprintf(fps_str, "fps: %d", engine_runtime_state.fps);
    sprintf(paint_time_str, "paint time: %dms", engine_runtime_state.frame_time);
    sprintf(entity_count_str, "visible entities: %d/%d", engine_runtime_state.painted_entity_count, engine_runtime_state.entity_count);
    sprintf(audio_chunk_count_str, "audio chunk count: %d", engine_runtime_state.audio_chunk_count);
    sprintf(log_line_count_str, "log line count: %d", engine_runtime_state.log_line_count);
    
    if (nk_begin(ctx, "Metrics", nk_rect(10, 10, 220, 200),
                    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)) {
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_label(ctx, fps_str, NK_TEXT_LEFT);
        nk_label(ctx, paint_time_str, NK_TEXT_LEFT);
        nk_label(ctx, entity_count_str, NK_TEXT_LEFT);
        nk_label(ctx, audio_chunk_count_str, NK_TEXT_LEFT);
        nk_label(ctx, log_line_count_str, NK_TEXT_LEFT);
    }
    nk_end(ctx);
}

/*
    paint information on the current camera, x,y,w,h,z
*/
void ui_paint_cam_info(){
    char x_str[100];
    char y_str[100];
    char w_str[100];
    char h_str[100];
    char z_str[100];

    sprintf(x_str, "x: %f", engine_state.target_camera->transform->rect.x);
    sprintf(y_str, "y: %f", engine_state.target_camera->transform->rect.y);
    sprintf(w_str, "w: %d", engine_state.target_camera->camera->view_field.w);
    sprintf(h_str, "h: %d", engine_state.target_camera->camera->view_field.h);
    sprintf(z_str, "z: %d", engine_state.target_camera->transform->z);

    // Create the GUI layout
    if (nk_begin(ctx, "Camera", nk_rect(250, 10, 100, 200),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, x_str, NK_TEXT_LEFT);
            nk_label(ctx, y_str, NK_TEXT_LEFT);
            nk_label(ctx, w_str, NK_TEXT_LEFT);
            nk_label(ctx, h_str, NK_TEXT_LEFT);
            nk_label(ctx, z_str, NK_TEXT_LEFT);
    nk_end(ctx);
    }
}

/*
    Will iterate and render all tracked ui components by calling their function pointers

    These functions cannot take parameters (other than the Nuklear context), 
    I would reccomend just scoping them to observe local variables in the files they reside in
*/
void ui_render(){
    // render all tracked ui components
    for (int i = 0; i < num_ui_components; i++) {
        UIComponent* component = &ui_components[i];
        if (component->render_function != NULL) {
            component->render_function(ctx);
        }
    }

    // paint everything
    nk_sdl_render(NK_ANTI_ALIASING_ON);
}

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
    struct nk_font_atlas *atlas;
    struct nk_font_config config = nk_font_config(0);
    struct nk_font *font;

    /* set up the font atlas and add desired font; note that font sizes are
        * multiplied by font_scale to produce better results at higher DPIs */
    nk_sdl_font_stash_begin(&atlas);
    // font = nk_font_atlas_add_from_file(atlas, ye_get_engine_resource_static("Orbitron-Regular.ttf"), 20 * font_scale, &config);
    font = nk_font_atlas_add_from_file(atlas, ye_get_engine_resource_static("RobotoMono-Regular.ttf"), 20 * font_scale, &config);
    nk_sdl_font_stash_end();

    /* this hack makes the font appear to be scaled down to the desired
        * size and is only necessary when font_scale > 1 */
    font->handle.height /= font_scale;
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
    nk_style_set_font(ctx, &font->handle);

    // set_style(ctx, THEME_DARK); TODO: might use a custom theme later on

    // ui_register_component("test",paint_test);
    if(engine_state.debug_mode){
        ui_register_component("debug_overlay",ui_paint_debug_overlay);
        ui_register_component("cam_info",ui_paint_cam_info);
    }
    ye_logf(info, "ui initialized");
}

void shutdown_ui(){
    nk_sdl_shutdown();
    ye_logf(info, "Shut down UI\n");
}