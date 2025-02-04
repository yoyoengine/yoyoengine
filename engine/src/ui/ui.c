/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
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
#include <stdbool.h>

#include <SDL.h>

#include <yoyoengine/ui/ui.h>
#include <yoyoengine/event.h>
#include <yoyoengine/ui/overlays.h>

#include <yoyoengine/ye_nk.h>

#include <yoyoengine/yep.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/logging.h>
#include <yoyoengine/ecs/camera.h>
#include <yoyoengine/ecs/transform.h>

#define MAX_UI_COMPONENTS 30
#define MAX_KEY_LENGTH 100

float font_scale = 1;

struct nk_context *ctx;

struct nk_font *_ye_font_p;
struct nk_font *_ye_font_h1;
struct nk_font *_ye_font_h2;
struct nk_font *_ye_font_h3;

#define YE_FONT(name) nk_style_set_font(ctx, &name->handle)

void ye_font_p(){ YE_FONT(_ye_font_p); }
void ye_font_h1(){ YE_FONT(_ye_font_h1); }
void ye_font_h2(){ YE_FONT(_ye_font_h2); }
void ye_font_h3(){ YE_FONT(_ye_font_h3); }

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

void ui_toggle_component(char* key, void (*render_function)()) {
    if (ui_component_exists(key)) {
        remove_ui_component(key);
    } else {
        ui_register_component(key, render_function);
    }
}

bool ui_component_exists(char *key) {
    for (int i = 0; i < num_ui_components; i++) {
        UIComponent* component = &ui_components[i];
        if (strcmp(component->key, key) == 0) {
            return true;
        }
    }
    return false;
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

// used to represent debug chart data in ui.c
struct ye_debug_chart_value {
    int ticks;
    float value;
};

struct ye_debug_chart_value framerate_log[60];
int framerate_log_index = 0;
int last_updated_framerate_log = 0;

void ui_paint_debug_overlay(struct nk_context *ctx){

    // put all the parameters into strings for display
    char fps_str[100];
    char render_call_count_str[100];
    char vertex_count_str[100];
    char event_count_str[100];
    char input_time_str[100];
    char physics_time_str[100];
    char paint_time_str[100];
    char frame_time_str[100];
    char delta_time_str[100];

    char entity_count_str[100];
    char audio_chunk_count_str[100];
    char log_line_count_str[100];
    sprintf(fps_str, "fps: %d", YE_STATE.runtime.fps);
    sprintf(render_call_count_str, "render calls: %d", YE_STATE.runtime.render_v2.num_render_calls);
    sprintf(vertex_count_str, "vertex count: %d", YE_STATE.runtime.render_v2.num_verticies);
    sprintf(event_count_str, "event count: %d", ye_get_num_events());
    sprintf(input_time_str, "input time: %dms", YE_STATE.runtime.input_time);
    sprintf(physics_time_str, "physics time: %dms", YE_STATE.runtime.physics_time);
    sprintf(paint_time_str, "paint time: %dms", YE_STATE.runtime.paint_time);
    sprintf(frame_time_str, "frame time: %dms", YE_STATE.runtime.frame_time);
    sprintf(delta_time_str, "delta time: %f", YE_STATE.runtime.delta_time);
    
    sprintf(entity_count_str, "entity count: %d", YE_STATE.runtime.entity_count);
    sprintf(audio_chunk_count_str, "audio chunk count: %d", YE_STATE.runtime.audio_chunk_count);
    sprintf(log_line_count_str, "log line count: %d", YE_STATE.runtime.log_line_count);

    // update chart logs

    int ticks = SDL_GetTicks();

    // fps chart logs
    if(ticks - last_updated_framerate_log > 100){
        framerate_log[framerate_log_index].ticks = ticks;
        framerate_log[framerate_log_index].value = YE_STATE.runtime.fps;
        framerate_log_index++;
        if(framerate_log_index >= 60){
            framerate_log_index = 0;
        }
        last_updated_framerate_log = ticks;
    }

    // paint gui
    if (nk_begin(ctx, "Metrics", nk_rect(10, 10, 230, 300),
                    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)) {
        
        // fps chart
        nk_layout_row_dynamic(ctx, 50, 1);

        struct nk_color chart_color;
        if(YE_STATE.runtime.fps < 30){
            chart_color = nk_rgb(255, 0, 0);
        }
        else if(YE_STATE.runtime.fps < 60){
            chart_color = nk_rgb(255, 255, 0);
        }
        else{
            chart_color = nk_rgb(0, 255, 0);
        }

        if(nk_chart_begin_colored(ctx, NK_CHART_LINES, chart_color, nk_rgb(255,255,255), 60, 0, 144)){
            for(int i = framerate_log_index; i < framerate_log_index + 60; i++){
                int current_index = i % 60;
                nk_flags res = nk_chart_push(ctx, framerate_log[current_index].value);
                if(res & NK_CHART_HOVERING){
                    if(nk_tooltip_begin(ctx, 100)){
                        nk_layout_row_dynamic(ctx, 20, 1);

                        char value_str[100];
                        sprintf(value_str, "FPS: %.2f", framerate_log[current_index].value);
                        nk_label(ctx, value_str, NK_TEXT_LEFT);
                        
                        char time_str[100];
                        sprintf(time_str, "Ticks: %d", framerate_log[current_index].ticks);
                        nk_label(ctx, time_str, NK_TEXT_LEFT);

                        nk_tooltip_end(ctx);
                    }
                }
            }
            nk_chart_end(ctx);
        }

        nk_layout_row_dynamic(ctx, 25, 1);
        nk_label(ctx, fps_str, NK_TEXT_LEFT);
        nk_label(ctx, render_call_count_str, NK_TEXT_LEFT);
        nk_label(ctx, vertex_count_str, NK_TEXT_LEFT);
        nk_label(ctx, event_count_str, NK_TEXT_LEFT);
        nk_label(ctx, input_time_str, NK_TEXT_LEFT);
        nk_label(ctx, physics_time_str, NK_TEXT_LEFT);
        nk_label(ctx, paint_time_str, NK_TEXT_LEFT);
        nk_label(ctx, frame_time_str, NK_TEXT_LEFT);
        nk_label(ctx, delta_time_str, NK_TEXT_LEFT);

        nk_label(ctx, entity_count_str, NK_TEXT_LEFT);
        nk_label(ctx, audio_chunk_count_str, NK_TEXT_LEFT);
        nk_label(ctx, log_line_count_str, NK_TEXT_LEFT);
    }
    nk_end(ctx);
}

void ui_paint_cam_info(struct nk_context *ctx){
    char x_str[100];
    char y_str[100];
    char w_str[100];
    char h_str[100];
    char z_str[100];

    if(YE_STATE.engine.target_camera == NULL){
        return;
    }

    sprintf(x_str, "x: %f", YE_STATE.engine.target_camera->transform->x);
    sprintf(y_str, "y: %f", YE_STATE.engine.target_camera->transform->y);
    sprintf(w_str, "w: %f", YE_STATE.engine.target_camera->camera->view_field.w);
    sprintf(h_str, "h: %f", YE_STATE.engine.target_camera->camera->view_field.h);
    sprintf(z_str, "z: %d", YE_STATE.engine.target_camera->camera->z);

    // Create the GUI layout
    if (nk_begin(ctx, "Camera", nk_rect(250, 10, 100, 200),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, x_str, NK_TEXT_LEFT);
            nk_label(ctx, y_str, NK_TEXT_LEFT);
            nk_label(ctx, w_str, NK_TEXT_LEFT);
            nk_label(ctx, h_str, NK_TEXT_LEFT);
            nk_label(ctx, z_str, NK_TEXT_LEFT);

        nk_property_float(ctx, "width", 0, &YE_STATE.engine.target_camera->camera->view_field.w, 90000, 1, 1);
        nk_property_float(ctx, "height", 0, &YE_STATE.engine.target_camera->camera->view_field.h, 90000, 1, 1);

    nk_end(ctx);
    }
}

void ui_render(){
    // render all tracked ui components
    for (int i = 0; i < num_ui_components; i++) {
        UIComponent* component = &ui_components[i];
        if (component->render_function != NULL) {
            component->render_function(ctx);
        }
    }

    // render all overlays
    ye_fire_overlay_event(YE_OVERLAY_EVENT_RENDER_UI);

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
        SDL_GetCurrentRenderOutputSize(renderer, &render_w, &render_h);
        SDL_GetWindowSize(win, &window_w, &window_h);
        scale_x = (float)(render_w) / (float)(window_w);
        scale_y = (float)(render_h) / (float)(window_h);
        SDL_SetRenderScale(renderer, scale_x, scale_y);
        font_scale = scale_y;
    }
    ctx = nk_sdl_init(win, renderer);
    /* GUI */
    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    struct nk_font_atlas *atlas;
    struct nk_font_config config = nk_font_config(0);

    /* set up the font atlas and add desired font; note that font sizes are
        * multiplied by font_scale to produce better results at higher DPIs */
    nk_sdl_font_stash_begin(&atlas);
    
    /*
        If in editor mode, we load from engine resource file. If at runtime load from the packed engine resource yep
    */
    if(YE_STATE.editor.editor_mode){
        _ye_font_p = nk_font_atlas_add_from_file(atlas, ye_get_engine_resource_static("fonts/RobotoMono-Regular.ttf"), 20 * font_scale, &config);
        _ye_font_h1 = nk_font_atlas_add_from_file(atlas, ye_get_engine_resource_static("fonts/RobotoMono-Regular.ttf"), 70 * font_scale, &config);
        _ye_font_h2 = nk_font_atlas_add_from_file(atlas, ye_get_engine_resource_static("fonts/RobotoMono-Regular.ttf"), 50 * font_scale, &config);
        _ye_font_h3 = nk_font_atlas_add_from_file(atlas, ye_get_engine_resource_static("fonts/RobotoMono-Regular.ttf"), 30 * font_scale, &config);
    }
    else{
        // get font binary data from engine resources
        struct yep_data_info font_data = yep_engine_resource_misc("fonts/RobotoMono-Regular.ttf");

        _ye_font_p = nk_font_atlas_add_from_memory(atlas, font_data.data, (nk_size)font_data.size, 20 * font_scale, &config);
        _ye_font_h1 = nk_font_atlas_add_from_memory(atlas, font_data.data, (nk_size)font_data.size, 70 * font_scale, &config);
        _ye_font_h2 = nk_font_atlas_add_from_memory(atlas, font_data.data, (nk_size)font_data.size, 50 * font_scale, &config);
        _ye_font_h3 = nk_font_atlas_add_from_memory(atlas, font_data.data, (nk_size)font_data.size, 30 * font_scale, &config);

        free(font_data.data); // GUESSING: nuklear seems to make its own copy of atlas when supplied this buffer so we are ok to free it
    }
    
    nk_sdl_font_stash_end();

    /* this hack makes the font appear to be scaled down to the desired
        * size and is only necessary when font_scale > 1 */
    _ye_font_p->handle.height /= font_scale;
    _ye_font_h1->handle.height /= font_scale;
    _ye_font_h2->handle.height /= font_scale;
    _ye_font_h3->handle.height /= font_scale;
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
    nk_style_set_font(ctx, &_ye_font_p->handle);

    // ui_register_component("test",paint_test);
    if(YE_STATE.engine.debug_mode){
        ui_register_component("debug_overlay",ui_paint_debug_overlay);
        ui_register_component("cam_info",ui_paint_cam_info);
    }

    YE_STATE.engine.ctx = ctx;

    ye_logf(info, "ui initialized\n");
}

void shutdown_ui(){
    nk_sdl_shutdown();
    ye_logf(info, "Shut down UI\n");
}