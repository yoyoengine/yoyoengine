/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <SDL.h>

#include <stdlib.h>

#include <yoyoengine/utils.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/debug_renderer.h>

struct ye_additional_render_callback_node * additional_render_head = NULL;
struct ye_debug_render_immediate_node * immediate_render_head = NULL;

/*
    TODO: malloc every frame is not ideal, but it's fine for now. Probably not even worth optimizing.
*/

/*
    immediate API
*/

void ye_debug_render_line(int x1, int y1, int x2, int y2, SDL_Color color, int width){
    struct ye_debug_render_immediate_node * new_node = malloc(sizeof(struct ye_debug_render_immediate_node));
    new_node->type = YE_DEBUG_RENDER_LINE;
    new_node->color = color;
    new_node->data.line.start = (SDL_Point){x1, y1};
    new_node->data.line.end = (SDL_Point){x2, y2};
    new_node->width = width;
    new_node->next = immediate_render_head;
    immediate_render_head = new_node;
}

void ye_debug_render_rect(int x, int y, int w, int h, SDL_Color color, int width){
    struct ye_debug_render_immediate_node * new_node = malloc(sizeof(struct ye_debug_render_immediate_node));
    new_node->type = YE_DEBUG_RENDER_RECT;
    new_node->color = color;
    new_node->data.rect = (SDL_Rect){x, y, w, h};
    new_node->width = width;
    new_node->next = immediate_render_head;
    immediate_render_head = new_node;
}

void ye_debug_render_circle(int x, int y, int radius, SDL_Color color, int width){
    struct ye_debug_render_immediate_node * new_node = malloc(sizeof(struct ye_debug_render_immediate_node));
    new_node->type = YE_DEBUG_RENDER_CIRCLE;
    new_node->color = color;
    new_node->data.circle.center = (SDL_Point){x, y};
    new_node->data.circle.radius = radius;
    new_node->width = width;
    new_node->next = immediate_render_head;
    immediate_render_head = new_node;
}

void ye_debug_render_point(int x, int y, SDL_Color color, int width){
    struct ye_debug_render_immediate_node * new_node = malloc(sizeof(struct ye_debug_render_immediate_node));
    new_node->type = YE_DEBUG_RENDER_POINT;
    new_node->color = color;
    new_node->data.point = (SDL_Point){x, y};
    new_node->width = width;
    new_node->next = immediate_render_head;
    immediate_render_head = new_node;
}

/*
    additional render callback API
*/

void ye_additional_render_fn_push(void (*fn)(void), bool persistent){
    struct ye_additional_render_callback_node * new_node = malloc(sizeof(struct ye_additional_render_callback_node));
    new_node->fn = fn;
    new_node->persistent = persistent;
    new_node->next = additional_render_head;
    additional_render_head = new_node;
}

/*
    Engine impl
*/

void ye_debug_renderer_render(){

    SDL_Renderer * renderer = YE_STATE.runtime.renderer;

    SDL_Rect camera_rect = ye_get_position_rect(YE_STATE.engine.target_camera,YE_COMPONENT_CAMERA);

    /*
        Immediate mode rendering
    */
    struct ye_debug_render_immediate_node * itr = immediate_render_head;
    while(itr != NULL){
        // set render color
        SDL_SetRenderDrawColor(renderer, itr->color.r, itr->color.g, itr->color.b, itr->color.a);
        
        switch (itr->type) {
            case YE_DEBUG_RENDER_LINE:
                // SDL_RenderDrawLine(renderer, itr->data.line.start.x - camera_rect.x, itr->data.line.start.y - camera_rect.y, itr->data.line.end.x - camera_rect.x, itr->data.line.end.y - camera_rect.y);
                ye_draw_thick_line(renderer, itr->data.line.start.x - camera_rect.x, itr->data.line.start.y - camera_rect.y, itr->data.line.end.x - camera_rect.x, itr->data.line.end.y - camera_rect.y, itr->width, itr->color);
                break;
            case YE_DEBUG_RENDER_RECT: {
                SDL_Rect rect = itr->data.rect;
                rect.x -= camera_rect.x;
                rect.y -= camera_rect.y;
                // SDL_RenderDrawRect(renderer, &rect);
                ye_draw_thick_rect(renderer, rect.x, rect.y, rect.w, rect.h, itr->width, itr->color);
                break;
            }
            case YE_DEBUG_RENDER_CIRCLE:
                ye_draw_circle(renderer, itr->data.circle.center.x - camera_rect.x, itr->data.circle.center.y - camera_rect.y, itr->data.circle.radius, itr->width);
                break;
            case YE_DEBUG_RENDER_POINT:
                // for points, we draw a pixel rect filled in
                ye_draw_thick_point(renderer, itr->data.point.x - camera_rect.x, itr->data.point.y - camera_rect.y, itr->width);
                break;
            default:
                break;
        }

        // free this node and move to the next
        struct ye_debug_render_immediate_node * temp = itr;
        itr = itr->next;
        free(temp);
        immediate_render_head = itr;
    }

    /*
        Callback additional rendering
    */
    struct ye_additional_render_callback_node * itr_fn = additional_render_head;
    while(itr_fn != NULL){
        itr_fn->fn();
        itr_fn = itr_fn->next;
    }
}

void ye_debug_renderer_cleanup(bool remove_persistant){
    struct ye_debug_render_immediate_node * itr = immediate_render_head;
    while(itr != NULL){
        struct ye_debug_render_immediate_node * temp = itr;
        itr = itr->next;
        free(temp);
    }
    immediate_render_head = NULL;

    struct ye_additional_render_callback_node * itr_fn = additional_render_head;
    while(itr_fn != NULL){
        struct ye_additional_render_callback_node * temp = itr_fn;
        itr_fn = itr_fn->next;
        if(remove_persistant || !temp->persistent){
            free(temp);
        }
    }
    additional_render_head = NULL;
}