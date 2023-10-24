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
#include <yoyoengine/yoyoengine.h>
#include "editor.h"

const float ratio[] = {0.03f, 0.92f, 0.05};
void ye_editor_paint_hiearchy(struct nk_context *ctx){
    // if no selected entity its height will be full height, else its half
    int height = YE_STATE.editor.selected_entity == NULL ? screenHeight : screenHeight/2;
    if (nk_begin(ctx, "Heiarchy", nk_rect(screenWidth/1.5, 0, screenWidth - screenWidth/1.5, height),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Controls:", NK_TEXT_LEFT);
            if(nk_button_label(ctx, "New Entity")){
                ye_create_entity();
                entity_list_head = ye_get_entity_list_head();
            }

            nk_label(ctx, "Entities:", NK_TEXT_LEFT);

            nk_layout_row_dynamic(ctx, 25, 3);
            nk_label(ctx, "Active", NK_TEXT_LEFT);
            nk_label(ctx, "Name", NK_TEXT_CENTERED);
            nk_label(ctx, "Delete", NK_TEXT_RIGHT);

            // iterate through entity_list_head and display the names of each entity as a button
            struct ye_entity_node *current = entity_list_head;
            while(current != NULL){
                /*
                    Honestly, should just leave editor camera in the heiarchy for fun lol
                    Kinda funny that you could just nuke it if you wanted
                */
                if(current->entity == editor_camera || current->entity == origin){
                    current = current->next;
                    continue;
                }
                nk_layout_row(ctx, NK_DYNAMIC, 25, 3, ratio);
                nk_checkbox_label(ctx, "", &current->entity->active);

                // if the entity is selected, display it as a different color
                bool flag = false; // messy way to do this, but it works
                if(YE_STATE.editor.selected_entity == current->entity){
                    nk_style_push_style_item(ctx, &ctx->style.button.normal, nk_style_item_color(nk_rgb(100,100,100))); nk_style_push_style_item(ctx, &ctx->style.button.hover, nk_style_item_color(nk_rgb(75,75,75))); nk_style_push_style_item(ctx, &ctx->style.button.active, nk_style_item_color(nk_rgb(50,50,50))); nk_style_push_vec2(ctx, &ctx->style.button.padding, nk_vec2(2,2));
                    flag = true;
                }
                else if(!current->entity->active){
                    nk_style_push_style_item(ctx, &ctx->style.button.normal, nk_style_item_color(nk_rgb(40,40,40))); nk_style_push_style_item(ctx, &ctx->style.button.hover, nk_style_item_color(nk_rgb(20,20,20))); nk_style_push_style_item(ctx, &ctx->style.button.active, nk_style_item_color(nk_rgb(75,75,75))); nk_style_push_vec2(ctx, &ctx->style.button.padding, nk_vec2(2,2));
                    flag = true;
                }

                if(nk_button_label(ctx, current->entity->name)){
                    if(YE_STATE.editor.selected_entity == current->entity){
                        YE_STATE.editor.selected_entity = NULL;
                        // pop our style items if we pushed them
                        if(flag){ // if we are selected, pop our style items
                            nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_vec2(ctx);
                        }
                        break;
                    }
                    YE_STATE.editor.selected_entity = current->entity;
                    entity_list_head = ye_get_entity_list_head();
                    // set all our current entity staging fields
                    staged_entity = *current->entity; // TODO this is hard because we have to copy all the components too... maybe we just need to let modification of fields directly and skip them if they are invalid
                    // pop our style items if we pushed them
                    if(flag){ // if we are selected, pop our style items
                        nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_vec2(ctx);
                    }
                    break;
                }

                // pop our style items if we pushed them
                if(flag){ // if we are selected, pop our style items
                    nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_vec2(ctx);
                }

                // push some pretty styles for red button!! (thank you nuklear forum!) :D
                nk_style_push_style_item(ctx, &ctx->style.button.normal, nk_style_item_color(nk_rgb(35,35,35))); nk_style_push_style_item(ctx, &ctx->style.button.hover, nk_style_item_color(nk_rgb(255,0,0))); nk_style_push_style_item(ctx, &ctx->style.button.active, nk_style_item_color(nk_rgb(255,0,0))); nk_style_push_vec2(ctx, &ctx->style.button.padding, nk_vec2(2,2));
                
                if(nk_button_symbol(ctx, NK_SYMBOL_X)){
                    // if our selected entity is the current entity, close the hiearchy
                    if(YE_STATE.editor.selected_entity == current->entity){
                        YE_STATE.editor.selected_entity = NULL;
                    }

                    ye_destroy_entity(current->entity);
                    entity_list_head = ye_get_entity_list_head();
                    nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_vec2(ctx);
                    break;
                }

                // pop off our cool red button colors
                nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_vec2(ctx);

                current = current->next;
            }
        nk_end(ctx);
    }
}

/*
    The entity preview will be a snapshot of the entity when selected, it will stash current state, and allow editing of the entity until
    dev clicks "save" or "cancel"
*/
void ye_editor_paint_entity(struct nk_context *ctx){
    struct ye_entity *ent = YE_STATE.editor.selected_entity;
    if(ent == NULL){
        return;
    }
    if (nk_begin(ctx, "Entity", nk_rect(screenWidth/1.5, screenHeight / 2, screenWidth - screenWidth/1.5, screenHeight),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
            if(ent == NULL){
                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label_colored(ctx, "No entity selected", NK_TEXT_CENTERED, nk_rgb(255, 255, 255));
            }
            else{
                nk_layout_row_dynamic(ctx, 25, 2);
                nk_label(ctx, "Name:", NK_TEXT_LEFT);
                // nk_label(ctx, ent->name, NK_TEXT_LEFT); // TODO: make modifiable

                nk_layout_row_dynamic(ctx, 25, 1);
                nk_checkbox_label(ctx, "Active", &ent->active);

                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label(ctx, "Components:", NK_TEXT_LEFT);
                
                if(ent->transform != NULL){
                    if(nk_tree_push(ctx, NK_TREE_TAB, "Transform", NK_MAXIMIZED)){
                        nk_layout_row_dynamic(ctx, 25, 4);
                        nk_label(ctx, "X:", NK_TEXT_CENTERED);
                        nk_property_float(ctx, "#x", -1000000, &ent->transform->rect.x, 1000000, 1, 5);
                        nk_label(ctx, "Y:", NK_TEXT_CENTERED);
                        nk_property_float(ctx, "#y", -1000000, &ent->transform->rect.y, 1000000, 1, 5);
                        nk_layout_row_dynamic(ctx, 25, 4);
                        nk_label(ctx, "W:", NK_TEXT_CENTERED);
                        nk_property_float(ctx, "#w", -1000000, &ent->transform->rect.w, 1000000, 1, 5);
                        nk_label(ctx, "W:", NK_TEXT_CENTERED);
                        nk_property_float(ctx, "#h", -1000000, &ent->transform->rect.h, 1000000, 1, 5);
                        nk_tree_pop(ctx);
                    }
                }

                if(ent->renderer != NULL){
                    if(nk_tree_push(ctx, NK_TREE_TAB, "Renderer", NK_MAXIMIZED)){
                        nk_label(ctx, "Test", NK_TEXT_LEFT);

                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_checkbox_label(ctx, "Flipped X", &ent->renderer->flipped_x);
                        nk_checkbox_label(ctx, "Flipped Y", &ent->renderer->flipped_y);
                        // ye_editor_paint_position(ctx, &ent->renderer->position); TODO: removeme, stashed attempt 1 rework
                        nk_tree_pop(ctx);
                    }
                }

                if(ent->physics != NULL){
                    if(nk_tree_push(ctx, NK_TREE_TAB, "Phyiscs", NK_MAXIMIZED)){
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Active", &ent->physics->active);
                        nk_layout_row_dynamic(ctx, 25, 4);
                        nk_label(ctx, "X Velocity:", NK_TEXT_CENTERED);
                        nk_property_float(ctx, "#xv", -1000000, &ent->physics->velocity.x, 1000000, 1, 5);
                        nk_label(ctx, "Y Velocity:", NK_TEXT_CENTERED);
                        nk_property_float(ctx, "#yv", -1000000, &ent->physics->velocity.y, 1000000, 1, 5);
                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_label(ctx, "Rotational Velocity:", NK_TEXT_CENTERED);
                        nk_property_float(ctx, "#rv", -1000000, &ent->physics->rotational_velocity, 1000000, 1, 5);
                        nk_tree_pop(ctx);
                    }
                }

                if(ent->collider != NULL){
                    if(nk_tree_push(ctx, NK_TREE_TAB, "Collider", NK_MAXIMIZED)){
                        nk_label(ctx, "Test", NK_TEXT_LEFT);
                        nk_tree_pop(ctx);
                    }
                }

                if(ent->tag != NULL){
                    if(nk_tree_push(ctx, NK_TREE_TAB, "Tag", NK_MAXIMIZED)){
                        // tag components can hold 10 buffers (TODO: sync this somehow with the #define in engine) so we want to just show them all as editable text boxes
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_label(ctx, "Tag Buffers:", NK_TEXT_LEFT);
                        for(int i = 0; i < 10;){
                            nk_layout_row_dynamic(ctx, 25, 2);
                            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, ent->tag->tags[i], 20, nk_filter_default); i++;
                            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, ent->tag->tags[i], 20, nk_filter_default); i++;
                        }
                        nk_tree_pop(ctx);
                    }
                }

                if(ent->camera != NULL){
                    if(nk_tree_push(ctx, NK_TREE_TAB, "Camera", NK_MAXIMIZED)){
                        nk_label(ctx, "Test", NK_TEXT_LEFT);
                        nk_tree_pop(ctx);
                    }
                }

                // save or cancel buttons
                nk_layout_row_dynamic(ctx, 25, 2);
                if(nk_button_label(ctx, "Save")){}
                if(nk_button_label(ctx, "Cancel")){
                    ent = NULL;
                }
            }
        nk_end(ctx);
    }
}


/*
    Paint some info on the current scene
*/
void ye_editor_paint_info_overlay(struct nk_context *ctx){
    if (nk_begin(ctx, "Info", nk_rect(0, 0, 200, 200),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Mouse World Pos:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            char buf[32];
            sprintf(buf, "%d,%d", mouse_world_x, mouse_world_y);
            nk_label(ctx, buf, NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Scene Name:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, YE_STATE.runtime.scene_name, NK_TEXT_LEFT);
        nk_end(ctx);
    }
}

/*
    TODO:
    - paintbounds somehow starts out checked even though its not, so have to double click to turn on
    - if console is opened when we tick one of these, closing the console crashes
*/
bool show_info_overlay = false;
void ye_editor_paint_options(struct nk_context *ctx){
    if (nk_begin(ctx, "Options", nk_rect(screenWidth/1.5 / 2, screenHeight/1.5, screenWidth - screenWidth/1.5, screenHeight - screenHeight/1.5),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            
            nk_label(ctx, "Overlays:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if(nk_checkbox_label(ctx, "Info", &show_info_overlay)){
                if(show_info_overlay){
                    ui_register_component("info_overlay",ye_editor_paint_info_overlay);
                }
                else{
                    remove_ui_component("info_overlay");
                }
            }

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Visual Debugging:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 2);
            nk_checkbox_label(ctx, "Display Names", &YE_STATE.editor.display_names);
            nk_checkbox_label(ctx, "Paintbounds", &YE_STATE.editor.paintbounds_visible);
            nk_checkbox_label(ctx, "Colliders", &YE_STATE.editor.colliders_visible);
            nk_checkbox_label(ctx, "Scene Camera Viewport", &YE_STATE.editor.scene_camera_bounds_visible);

            nk_label(ctx, "Preferences:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_checkbox_label(ctx, "Draw Lines", &YE_STATE.editor.editor_display_viewport_lines);

            nk_label(ctx, "Extra:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_checkbox_label(ctx, "Stretch Viewport", &YE_STATE.engine.stretch_viewport);
        nk_end(ctx);
    }
}
