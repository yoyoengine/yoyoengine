/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stdio.h>
#include <string.h>

#include <yoyoengine/engine.h>
#include <yoyoengine/ye_nk.h>
#include <yoyoengine/ui/chart.h>
#include <yoyoengine/overlays/physics_overlay.h>

void ye_physics_overlay_pre_frame_paint() {
    // printf("Physics overlay pre frame paint\n");
    // TODO
    return;
}
void ye_physics_overlay_post_frame_paint() {
    // printf("Physics overlay post frame paint\n");
    // TODO
    return;
}

struct ye_chart * physics_time_chart = NULL;
int chart_tick_delay_ms = 1;
int last_chart_tick = 0;

void ye_physics_overlay_init() {
    physics_time_chart = ye_new_chart("Physics Time", 240);
}

void ye_physics_overlay_shutdown() {
    ye_chart_destroy(physics_time_chart);
}

void ye_physics_overlay_render_ui_panel(struct nk_context *ctx) {
    if(nk_begin(ctx, "Physics Overlay", nk_rect(10, 30, 225, 375),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE)) {

        nk_layout_row_dynamic(ctx, 25, 2);
        nk_label(ctx, "Last Tick: ", NK_TEXT_LEFT);
        char buf[128];
        snprintf(buf, 128, "%d ms", YE_STATE.runtime.physics_time);
        nk_label(ctx, buf, NK_TEXT_RIGHT);

        nk_layout_row_dynamic(ctx, 25, 1);
        struct nk_color chart_color;
        if(YE_STATE.runtime.physics_time < 6){
            chart_color = nk_rgb(0, 255, 0);
        }
        else if(YE_STATE.runtime.physics_time < 8){
            chart_color = nk_rgb(255, 255, 0);
        }
        else{
            chart_color = nk_rgb(255, 0, 0);
        }
        if(nk_chart_begin_colored(ctx, NK_CHART_LINES, chart_color, nk_rgb(255,255,255), 240, 0, 16)){
            int c_ticks = SDL_GetTicks();
            if(c_ticks - last_chart_tick > chart_tick_delay_ms){
                ye_chart_push(physics_time_chart, YE_STATE.runtime.physics_time, c_ticks);
                last_chart_tick = c_ticks;
            }
            
            // Start from current index and move forward
            int start_index = physics_time_chart->index;
            
            for(int i = 0; i < physics_time_chart->size; i++) {
                int current_index = (start_index + i) % physics_time_chart->size;
                nk_flags res = nk_chart_push(ctx, physics_time_chart->values[current_index].value);
                if(res & NK_CHART_HOVERING){
                    if(nk_tooltip_begin(ctx, 250)){
                        nk_layout_row_dynamic(ctx, 20, 1);

                        char value_str[100];
                        sprintf(value_str, "Physics Time: %.2f ms", physics_time_chart->values[current_index].value);
                        nk_label(ctx, value_str, NK_TEXT_LEFT);
                        
                        char time_str[100];
                        sprintf(time_str, "Ticks: %d", physics_time_chart->values[current_index].tick);
                        nk_label(ctx, time_str, NK_TEXT_LEFT);

                        nk_tooltip_end(ctx);
                    }
                }
            }
            nk_chart_end(ctx);
        }

        nk_layout_row_dynamic(ctx, 25, 1);
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_label_colored(ctx, "State", NK_TEXT_CENTERED, nk_rgb(0, 255, 0));
        
        nk_layout_row_dynamic(ctx, 25, 2);
        nk_label(ctx, "objects: ", NK_TEXT_RIGHT);
        snprintf(buf, 128, "%d", YE_STATE.engine.p2d_state->p2d_object_count);
        nk_label(ctx, buf, NK_TEXT_CENTERED);
        // todo: "active" object count and cull offscreen objects? - param

        nk_layout_row_dynamic(ctx, 25, 2);
        nk_label(ctx, "nodes: ", NK_TEXT_RIGHT);
        snprintf(buf, 128, "%d", YE_STATE.engine.p2d_state->p2d_world_node_count);
        nk_label(ctx, buf, NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 25, 2);
        nk_label(ctx, "checks: ", NK_TEXT_RIGHT);
        snprintf(buf, 128, "%d", YE_STATE.engine.p2d_state->p2d_contact_checks);
        nk_label(ctx, buf, NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 25, 2);
        nk_label(ctx, "contacts: ", NK_TEXT_RIGHT);
        snprintf(buf, 128, "%d", YE_STATE.engine.p2d_state->p2d_contacts_found);
        nk_label(ctx, buf, NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 25, 2);
        nk_label(ctx, "pairs: ", NK_TEXT_RIGHT);
        snprintf(buf, 128, "%d", YE_STATE.engine.p2d_state->p2d_collision_pairs);
        nk_label(ctx, buf, NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 25, 1);
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_label_colored(ctx, "Gravity", NK_TEXT_CENTERED, nk_rgb(0, 255, 0));
        snprintf(buf, 128, "<%f, %f>", YE_STATE.engine.p2d_state->gravity.x, YE_STATE.engine.p2d_state->gravity.y);
        nk_label(ctx, buf, NK_TEXT_CENTERED);

        nk_end(ctx);
    }
}
