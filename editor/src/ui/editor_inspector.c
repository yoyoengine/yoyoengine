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

// lowkey i just copied these all from editor_ui.c so maybe these are redundant
#include <stdio.h>
#include <unistd.h>
#include <yoyoengine/yoyoengine.h>
#include "editor.h"
#include "editor_ui.h"
#include "editor_serialize.h"
#include "editor_panels.h"

void _paint_transform(struct nk_context *ctx, struct ye_entity *ent){
    if(ent->transform != NULL){
        if(nk_tree_push(ctx, NK_TREE_TAB, "Transform", NK_MAXIMIZED)){
            nk_layout_row_dynamic(ctx, 25, 4);
            nk_label(ctx, "X:", NK_TEXT_CENTERED);
            nk_property_float(ctx, "#x", -1000000, &ent->transform->x, 1000000, 1, 5);
            nk_label(ctx, "Y:", NK_TEXT_CENTERED);
            nk_property_float(ctx, "#y", -1000000, &ent->transform->y, 1000000, 1, 5);
            
            nk_layout_row_dynamic(ctx, 25, 1);
            if(nk_button_label(ctx, "Remove Component")){
                ye_remove_transform_component(ent);
            }

            nk_tree_pop(ctx);
        }
    }
}

void _paint_renderer(struct nk_context *ctx, struct ye_entity *ent){
    if(ent->renderer != NULL){
        if(nk_tree_push(ctx, NK_TREE_TAB, "Renderer", NK_MAXIMIZED)){
            nk_layout_row_dynamic(ctx, 25, 2);
            nk_checkbox_label(ctx, "Active", (nk_bool*)&ent->renderer->active);
            nk_checkbox_label(ctx, "Relative", (nk_bool*)&ent->renderer->relative);

            nk_layout_row_dynamic(ctx, 25, 2);
            nk_property_float(ctx, "#x", -1000000, &ent->renderer->rect.x, 1000000, 1, 5);
            nk_property_float(ctx, "#y", -1000000, &ent->renderer->rect.y, 1000000, 1, 5);
            nk_property_float(ctx, "#w", -1000000, &ent->renderer->rect.w, 1000000, 1, 5);
            nk_property_float(ctx, "#h", -1000000, &ent->renderer->rect.h, 1000000, 1, 5);

            nk_checkbox_label(ctx, "Flipped X", (nk_bool*)&ent->renderer->flipped_x);
            nk_checkbox_label(ctx, "Flipped Y", (nk_bool*)&ent->renderer->flipped_y);

            nk_layout_row_dynamic(ctx, 25, 2);
            // nk_label(ctx, "Alignment:", NK_TEXT_LEFT); TODO
            nk_property_int(ctx, "#z", -1000000, &ent->renderer->z, 1000000, 1, 5);
            nk_property_float(ctx, "#Rotation", -1000000, &ent->renderer->rotation, 1000000, 1, 5);

            if (nk_tree_push(ctx, NK_TREE_TAB, "Alignment", NK_MAXIMIZED))
            {
                int i;
                static int alignment_arr[9] = {0,0,0, 0,0,0, 0,0,0};
                for (int i = 0; i < 9; i++) {alignment_arr[i] = 0;}
                // ^ zero the arr then we load its value from actual comp property

                switch(YE_STATE.editor.selected_entity->renderer->alignment){
                    case 9:
                        // stretch, nothing selected
                        break;
                    default:
                        alignment_arr[YE_STATE.editor.selected_entity->renderer->alignment] = 1;
                        // printf("alignment: %d\n", YE_STATE.editor.selected_entity->renderer->alignment);
                        break;
                }

                nk_layout_row_static(ctx, 50, 100, 3);
                for (i = 0; i < 9; ++i) {
                    if (nk_selectable_label(ctx, "X", NK_TEXT_CENTERED, &alignment_arr[i])) {
                        // // zero any other fields that might have been selected
                        // for(int j = 0; j < 9; j++){
                        //     if(j != i){
                        //         alignment_arr[j] = 0;
                        //     }
                        // } TODO: removeme. leaving here because I think i might have to bugfix this soon
                        
                        // if the entire array is zero
                        int sum = 0;
                        for(int j = 0; j < 9; j++){
                            sum += alignment_arr[j];
                        }
                        if(sum == 0){
                            ent->renderer->alignment = YE_ALIGN_STRETCH;
                        }
                        else{
                            ent->renderer->alignment = (enum ye_alignment)(i);
                        }
                        // printf("Selected alignment %d\n", i);
                    }
                }
                switch(ent->renderer->alignment){ // TODO: consolidate, this is yucky
                    case 0:
                        nk_label(ctx, "Top Left", NK_TEXT_CENTERED);
                        break;
                    case 1:
                        nk_label(ctx, "Top Center", NK_TEXT_CENTERED);
                        break;
                    case 2:
                        nk_label(ctx, "Top Right", NK_TEXT_CENTERED);
                        break;
                    case 3:
                        nk_label(ctx, "Middle Left", NK_TEXT_CENTERED);
                        break;
                    case 4:
                        nk_label(ctx, "Middle Center", NK_TEXT_CENTERED);
                        break;
                    case 5:
                        nk_label(ctx, "Middle Right", NK_TEXT_CENTERED);
                        break;
                    case 6:
                        nk_label(ctx, "Bottom Left", NK_TEXT_CENTERED);
                        break;
                    case 7:
                        nk_label(ctx, "Bottom Center", NK_TEXT_CENTERED);
                        break;
                    case 8:
                        nk_label(ctx, "Bottom Right", NK_TEXT_CENTERED);
                        break;
                    case 9:
                        nk_label(ctx, "Stretch", NK_TEXT_CENTERED);
                        break;
                    default:
                        nk_label(ctx, "!!!Corrupted!!!", NK_TEXT_CENTERED);
                        break;
                }
                nk_tree_pop(ctx);
            }

            if (nk_tree_push(ctx, NK_TREE_TAB, "Renderer Data", NK_MAXIMIZED)){
                switch(ent->renderer->type){
                    case YE_RENDERER_TYPE_IMAGE:
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_label(ctx, "Image Renderer", NK_TEXT_CENTERED);
                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_label(ctx, "Image src:", NK_TEXT_LEFT);
                        nk_label(ctx, "TODO:", NK_TEXT_LEFT);
                        nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, ent->renderer->renderer_impl.image->src, strlen(ent->renderer->renderer_impl.image->src), nk_filter_default);
                        break;
                    case YE_RENDERER_TYPE_TEXT:
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_label(ctx, "Text Renderer", NK_TEXT_CENTERED);
                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_label(ctx, "Text:", NK_TEXT_LEFT);
                        nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, ent->renderer->renderer_impl.text->text, strlen(ent->renderer->renderer_impl.text->text), nk_filter_default);

                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_label(ctx, "Color (name):", NK_TEXT_LEFT);
                        // nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, ent->renderer->renderer_impl.text->text, strlen(ent->renderer->renderer_impl.text->text), nk_filter_default);
                        
                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_label(ctx, "Font (name):", NK_TEXT_LEFT);
                        // nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, ent->renderer->renderer_impl.text->text, strlen(ent->renderer->renderer_impl.text->text), nk_filter_default);
                        
                        break;
                    /*
                        Todo: rest of the renderer types
                    */
                    default:
                        nk_label(ctx, "!!!Corrupted - NO TYPE!!!", NK_TEXT_CENTERED);
                        break;
                }
                nk_tree_pop(ctx);
            }

            nk_layout_row_dynamic(ctx, 25, 1);
            if(nk_button_label(ctx, "Remove Component")){
                ye_remove_renderer_component(ent);
            }

            nk_tree_pop(ctx);
        }
    }
}

void _paint_camera(struct nk_context *ctx, struct ye_entity *ent){
    if(ent->camera != NULL){
        if(nk_tree_push(ctx, NK_TREE_TAB, "Camera", NK_MAXIMIZED)){
            nk_layout_row_dynamic(ctx, 25, 2);
            nk_checkbox_label(ctx, "Active", (nk_bool*)&ent->camera->active);
            nk_checkbox_label(ctx, "Relative", (nk_bool*)&ent->camera->relative);
            nk_layout_row_dynamic(ctx, 25, 2);
            nk_property_int(ctx, "#x", -1000000, &ent->camera->view_field.x, 1000000, 1, 5);
            nk_property_int(ctx, "#y", -1000000, &ent->camera->view_field.y, 1000000, 1, 5);
            nk_property_int(ctx, "#w", -1000000, &ent->camera->view_field.w, 1000000, 1, 5);
            nk_property_int(ctx, "#h", -1000000, &ent->camera->view_field.h, 1000000, 1, 5);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "#z", -1000000, &ent->camera->z, 1000000, 1, 5);
            
            nk_layout_row_dynamic(ctx, 25, 1);
            if(nk_button_label(ctx, "Remove Component")){
                ye_remove_camera_component(ent);
            }
            
            nk_tree_pop(ctx);
        }
    }
}

void _paint_collider(struct nk_context *ctx, struct ye_entity *ent){
    if(ent->collider != NULL){
        if(nk_tree_push(ctx, NK_TREE_TAB, "Collider", NK_MAXIMIZED)){
            nk_layout_row_dynamic(ctx, 25, 2);
            nk_checkbox_label(ctx, "Active", (nk_bool*)&ent->collider->active);
            nk_checkbox_label(ctx, "Relative", (nk_bool*)&ent->collider->relative);
            nk_layout_row_dynamic(ctx, 25, 2);
            nk_property_float(ctx, "#x", -1000000, &ent->collider->rect.x, 1000000, 1, 5);
            nk_property_float(ctx, "#y", -1000000, &ent->collider->rect.y, 1000000, 1, 5);
            nk_property_float(ctx, "#w", -1000000, &ent->collider->rect.w, 1000000, 1, 5);
            nk_property_float(ctx, "#h", -1000000, &ent->collider->rect.h, 1000000, 1, 5);
            
            nk_layout_row_dynamic(ctx, 25, 1);
            if(nk_button_label(ctx, "Remove Component")){
                ye_remove_collider_component(ent);
            }
            
            nk_tree_pop(ctx);
        }
    }
}

void _paint_physics(struct nk_context *ctx, struct ye_entity *ent){
    if(ent->physics != NULL){
        if(nk_tree_push(ctx, NK_TREE_TAB, "Phyiscs", NK_MAXIMIZED)){
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_checkbox_label(ctx, "Active", (nk_bool*)&ent->physics->active);
            nk_layout_row_dynamic(ctx, 25, 4);
            nk_label(ctx, "X Velocity:", NK_TEXT_CENTERED);
            nk_property_float(ctx, "#xv", -1000000, &ent->physics->velocity.x, 1000000, 1, 5);
            nk_label(ctx, "Y Velocity:", NK_TEXT_CENTERED);
            nk_property_float(ctx, "#yv", -1000000, &ent->physics->velocity.y, 1000000, 1, 5);
            nk_layout_row_dynamic(ctx, 25, 2);
            nk_label(ctx, "Rotational Velocity:", NK_TEXT_CENTERED);
            nk_property_float(ctx, "#rv", -1000000, &ent->physics->rotational_velocity, 1000000, 1, 5);
            
            nk_layout_row_dynamic(ctx, 25, 1);
            if(nk_button_label(ctx, "Remove Component")){
                ye_remove_physics_component(ent);
            }
            
            nk_tree_pop(ctx);
        }
    }
}

void _paint_tag(struct nk_context *ctx, struct ye_entity *ent){
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

            nk_layout_row_dynamic(ctx, 25, 1);
            if(nk_button_label(ctx, "Remove Component")){
                ye_remove_tag_component(ent);
            }

            nk_tree_pop(ctx);
        }
    }
}

bool comp_exists(int i, struct ye_entity *ent){
    switch(i){
        case 0:
            return ent->transform != NULL;
            break;
        case 1:
            return ent->renderer != NULL;
            break;
        case 2:
            return ent->camera != NULL;
            break;
        case 3:
            return ent->collider != NULL;
            break;
        case 4:
            return ent->physics != NULL;
            break;
        case 5:
            return ent->tag != NULL;
            break;
        case 6:
            return ent->script != NULL;
            break;
        default:
            return false;
            break;
    }
}

/*
    inspector panel

    defined in editor_ui.h
*/
void ye_editor_paint_inspector(struct nk_context *ctx){
    struct ye_entity *ent = YE_STATE.editor.selected_entity;
    if(ent == NULL){
        return;
    }
    if (nk_begin(ctx, "Entity", nk_rect(screenWidth/1.5, screenHeight / 3, screenWidth - screenWidth/1.5, screenHeight - screenHeight/3),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
            if(ent == NULL){
                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label_colored(ctx, "No entity selected", NK_TEXT_CENTERED, nk_rgb(255, 255, 255));
            }
            else{
                nk_layout_row_dynamic(ctx, 25, 2);
                nk_label(ctx, "Name:", NK_TEXT_LEFT);
                // TODO: bugfix name editing, setting to zero len is unhappy, also should be 99 for str term?
                nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, ent->name, 100, nk_filter_default);

                nk_layout_row_dynamic(ctx, 25, 1);
                nk_checkbox_label(ctx, "Active", (nk_bool*)&ent->active);

                nk_layout_row_dynamic(ctx, 25, 1);
                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label(ctx, "Components:", NK_TEXT_LEFT);

                /*
                    Selector tile layout thing that shows all components in list
                */
                static int current_component_inspector_tab = 0;
                const char *names[] = {"Transform", "Renderer", "Camera","Collider","Physics","Tag","Script"};
                static int num_components = sizeof(names) / sizeof(names[0]); // does this mean its only computed here once?

                nk_style_push_vec2(ctx, &ctx->style.window.spacing, nk_vec2(0,0));
                nk_style_push_float(ctx, &ctx->style.button.rounding, 0);
                nk_layout_row_dynamic(ctx, 50, 7);
                for (int i = 0; i < num_components; ++i) {
                    // Check if the component is not null
                    if (comp_exists(i, ent)) {
                        // Change the text color to green
                        ctx->style.button.text_normal = nk_rgb(0,255,0);
                    } else {
                        // Reset the text color
                        ctx->style.button.text_normal = nk_rgb(255,255,255);
                    }

                    if (current_component_inspector_tab == i) {
                        /* active tab gets highlighted */
                        struct nk_style_item button_color = ctx->style.button.normal;
                        ctx->style.button.normal = ctx->style.button.active;
                        current_component_inspector_tab = nk_button_label(ctx, names[i]) ? i: current_component_inspector_tab;
                        ctx->style.button.normal = button_color;
                    } else current_component_inspector_tab = nk_button_label(ctx, names[i]) ? i: current_component_inspector_tab;
                }
                nk_style_pop_float(ctx);
                nk_style_pop_vec2(ctx);

                /* Im going to switch with the integers here, not because strcmp is too much overhead but because I cant be fucked to type allat */
                switch(current_component_inspector_tab){
                    case 0: // transform //
                        _paint_transform(ctx,ent);
                        break;
                    case 1: // renderer //
                        _paint_renderer(ctx,ent);
                        break;
                    case 2: // camera //
                        _paint_camera(ctx,ent);
                        break;
                    case 3: // collider //
                        _paint_collider(ctx,ent);
                        break;
                    case 4: // physics //
                        _paint_physics(ctx,ent);
                        break;
                    case 5: // tag //
                        _paint_tag(ctx,ent);
                        break;
                    case 6: // script //
                        break;
                }
            }
        nk_end(ctx);
    }
}