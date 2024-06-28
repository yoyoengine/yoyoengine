/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
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

#include <math.h>
#ifndef M_PI
#define M_PI 3.14159
#endif

#include <SDL.h>

#include <yoyoengine/utils.h>
#include <yoyoengine/graphics.h>
#include <yoyoengine/ecs/camera.h>
#include <yoyoengine/ecs/button.h>
#include <yoyoengine/ecs/collider.h>
#include <yoyoengine/ecs/renderer.h>
#include <yoyoengine/ecs/transform.h>
#include <yoyoengine/ecs/audiosource.h>

void ye_auto_fit_bounds(struct ye_rectf* bounds_f, struct ye_rectf* obj_f, enum ye_alignment alignment, SDL_Point* center, bool should_grow_to_fit){
    SDL_Rect _bounds = ye_convert_rectf_rect(*bounds_f);
    SDL_Rect _obj = ye_convert_rectf_rect(*obj_f);
    SDL_Rect * bounds = &_bounds;
    SDL_Rect * obj = &_obj;

    // check if some loser wants to stretch something
    if(alignment == YE_ALIGN_STRETCH){
        obj->x = bounds->x;
        obj->y = bounds->y;
        obj->w = bounds->w;
        obj->h = bounds->h;
        *center = (SDL_Point){obj->w / 2, obj->h / 2};
        *bounds_f = ye_convert_rect_rectf(*bounds);
        *obj_f = ye_convert_rect_rectf(*obj);
        return;
    }

    // actual orientation handling

    float boundsAspectRatio = (float)bounds->w / (float)bounds->h;
    float objectAspectRatio = (float)obj->w / (float)obj->h;

    if (should_grow_to_fit) {
        if (objectAspectRatio > boundsAspectRatio) {
            obj->h = bounds->w / objectAspectRatio;
            obj->w = bounds->w;
        } else {
            obj->w = bounds->h * objectAspectRatio;
            obj->h = bounds->h;
        }
    }

    switch(alignment) {
        case YE_ALIGN_TOP_LEFT:
            obj->x = bounds->x;
            obj->y = bounds->y;
            break;
        case YE_ALIGN_TOP_CENTER:
            obj->x = bounds->x + (bounds->w - obj->w) / 2;
            obj->y = bounds->y;
            break;
        case YE_ALIGN_TOP_RIGHT:
            obj->x = bounds->x + (bounds->w - obj->w);
            obj->y = bounds->y;
            break;
        case YE_ALIGN_MID_LEFT:
            obj->x = bounds->x;
            obj->y = bounds->y + (bounds->h - obj->h) / 2;
            break;
        case YE_ALIGN_MID_CENTER:
            obj->x = bounds->x + (bounds->w - obj->w) / 2;
            obj->y = bounds->y + (bounds->h - obj->h) / 2;
            break;
        case YE_ALIGN_MID_RIGHT:
            obj->x = bounds->x + (bounds->w - obj->w);
            obj->y = bounds->y + (bounds->h - obj->h) / 2;
            break;
        case YE_ALIGN_BOT_LEFT:
            obj->x = bounds->x;
            obj->y = bounds->y + (bounds->h - obj->h);
            break;
        case YE_ALIGN_BOT_CENTER:
            obj->x = bounds->x + (bounds->w - obj->w) / 2;
            obj->y = bounds->y + (bounds->h - obj->h);
            break;
        case YE_ALIGN_BOT_RIGHT:
            obj->x = bounds->x + (bounds->w - obj->w);
            obj->y = bounds->y + (bounds->h - obj->h);
            break;
        default:
            ye_logf(error, "Invalid alignment\n");
            break;
    }
    *center = (SDL_Point){obj->w / 2, obj->h / 2};

    *bounds_f = ye_convert_rect_rectf(*bounds);
    *obj_f = ye_convert_rect_rectf(*obj);
}

int ye_clamp(int value, int min, int max) {
    return (value < min) ? min : (value > max) ? max : value;
}

SDL_Rect ye_get_real_texture_size_rect(SDL_Texture *pTexture){
    int imgWidth, imgHeight;
    SDL_QueryTexture(pTexture, NULL, NULL, &imgWidth, &imgHeight);
    SDL_Rect rect = {0,0,imgWidth,imgHeight};
    return rect;
}

SDL_Rect ye_convert_rectf_rect(struct ye_rectf rect){
    SDL_Rect newRect = {rect.x, rect.y, rect.w, rect.h};
    return newRect;
}

struct ye_rectf ye_convert_rect_rectf(SDL_Rect rect){
    struct ye_rectf newRect = {rect.x, rect.y, rect.w, rect.h};
    return newRect;
}

/*
    Helper method that takes in two points for floats and returns the distance between them
*/
float ye_distance(float x1, float y1, float x2, float y2){
    float x = x2 - x1;
    float y = y2 - y1;
    return sqrtf(x * x + y * y);
}

/*
    Helper method that takes in two points a src and a dest and returns the angle between them, with 0 being north and increasing clockwise
*/
float ye_angle(float x1, float y1, float x2, float y2){
    float angle = atan2(y2 - y1, x2 - x1) * 180 / M_PI;
    if(angle < 0){
        angle += 360;
    }
    return angle;
}

struct ye_rectf ye_get_position(struct ye_entity *entity, enum ye_component_type type){
    if(entity == NULL){
        ye_logf(error, "Tried to get position for null entity \"%s\". returning (0,0,0,0)\n",entity->name);
        return (struct ye_rectf){0,0,0,0};
    }

    struct ye_rectf pos = {0,0,0,0};

    switch(type){
        case YE_COMPONENT_TRANSFORM:
            pos.x = entity->transform->x;
            pos.y = entity->transform->y;
            return pos;
        case YE_COMPONENT_RENDERER:
            if(entity->renderer != NULL){
                // set x,y,w,h
                pos = entity->renderer->rect;
                pos.w = entity->renderer->rect.w;
                pos.h = entity->renderer->rect.h;

                // if relative adjust its position
                if(entity->renderer->relative && entity->transform != NULL){
                    pos.x += entity->transform->x;
                    pos.y += entity->transform->y;
                }

                return pos;
            }
            ye_logf(error,"Tried to get position of a null renderer component on entity \"%s\". returning (0,0,0,0)\n",entity->name);
            return pos;
        case YE_COMPONENT_CAMERA:
            if(entity->camera != NULL){
                // set x,y,w,h
                pos = ye_convert_rect_rectf(entity->camera->view_field);

                // if relative adjust its position
                if(entity->camera->relative && entity->transform != NULL){
                    pos.x += entity->transform->x;
                    pos.y += entity->transform->y;
                }

                return pos;
            }
            ye_logf(error,"Tried to get position of a null camera component on entity \"%s\". returning (0,0,0,0)\n",entity->name);
            return pos;
        case YE_COMPONENT_COLLIDER:
            if(entity->collider != NULL){
                // set x,y,w,h
                pos = entity->collider->rect;
                pos.w = entity->collider->rect.w;
                pos.h = entity->collider->rect.h;

                // printf("UTIL: found pos at x:%f y:%f w:%f h:%f\n",pos.x,pos.y,pos.w,pos.h);

                // if relative adjust its position
                if(entity->collider->relative && entity->transform != NULL){
                    pos.x += entity->transform->x;
                    pos.y += entity->transform->y;
                }

                // printf("UTIL: adjusted pos at x:%f y:%f w:%f h:%f\n",pos.x,pos.y,pos.w,pos.h);

                return pos;
            }
            ye_logf(error,"Tried to get position of a null collider component on entity \"%s\". returning (0,0,0,0)\n",entity->name);
            return pos;
        case YE_COMPONENT_AUDIOSOURCE:
            if(entity->audiosource != NULL){
                // set x,y,w,h
                pos = entity->audiosource->range;
                pos.w = entity->audiosource->range.w;
                pos.h = entity->audiosource->range.h;

                // if relative adjust its position
                if(entity->audiosource->relative && entity->transform != NULL){
                    pos.x += entity->transform->x;
                    pos.y += entity->transform->y;
                }

                return pos;
            }
            ye_logf(error,"Tried to get position of a null collider component on entity \"%s\". returning (0,0,0,0)\n",entity->name);
            return pos;
        case YE_COMPONENT_BUTTON:
            if(entity->button != NULL){
                // set x,y,w,h
                pos = entity->button->rect;
                pos.w = entity->button->rect.w;
                pos.h = entity->button->rect.h;

                // if relative adjust its position
                if(entity->button->relative && entity->transform != NULL){
                    pos.x += entity->transform->x;
                    pos.y += entity->transform->y;
                }

                return pos;
            }
            ye_logf(error,"Tried to get position of a null button component on entity \"%s\". returning (0,0,0,0)\n",entity->name);
            return pos;
        default:
            ye_logf(error, "Tried to get position for component on \"%s\" that does not have a position or size. returning (0,0,0,0)\n",entity->name);
            return pos;
    }
}

SDL_Rect ye_get_position_rect(struct ye_entity *entity, enum ye_component_type type){
    struct ye_rectf pos = ye_get_position(entity, type);
    SDL_Rect rect = {pos.x, pos.y, pos.w, pos.h};
    return rect;
}

void ye_draw_thick_point(SDL_Renderer* renderer, int x, int y, int thickness) {
    int half_thickness = thickness / 2;
    SDL_Rect rect = {
        x - half_thickness,
        y - half_thickness,
        thickness,
        thickness
    };

    SDL_RenderFillRect(renderer, &rect);
}

/*
    credit: https://stackoverflow.com/questions/38334081/how-to-draw-circles-arcs-and-vector-graphics-in-sdl
*/
void ye_draw_circle(SDL_Renderer * renderer, int32_t center_x, int32_t center_y, int32_t radius, int thickness)
{
    const int32_t diameter = (radius * 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y)
    {
        //  Each of the following renders an octant of the circle
        ye_draw_thick_point(renderer, center_x + x, center_y - y, thickness);
        ye_draw_thick_point(renderer, center_x + x, center_y + y, thickness);
        ye_draw_thick_point(renderer, center_x - x, center_y - y, thickness);
        ye_draw_thick_point(renderer, center_x - x, center_y + y, thickness);
        ye_draw_thick_point(renderer, center_x + y, center_y - x, thickness);
        ye_draw_thick_point(renderer, center_x + y, center_y + x, thickness);
        ye_draw_thick_point(renderer, center_x - y, center_y - x, thickness);
        ye_draw_thick_point(renderer, center_x - y, center_y + x, thickness);

        if (error <= 0)
        {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0)
        {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}

void ye_get_mouse_world_position(int *x, int *y){
    // get the true world position of the camera
    struct ye_rectf campos = ye_get_position(YE_STATE.engine.target_camera, YE_COMPONENT_CAMERA);

    // get the viewable screen dimensions
    int viewableWidth, viewableHeight;

    // find the scale (how much we blow up or shrink the output to the window based on camera size)
    float scaleX, scaleY;

    // get the true screen size of the display
    struct ScreenSize screenSize = ye_get_screen_size();

    if(!YE_STATE.engine.need_boxing){
        // if we aren't using letterboxing, use the fullscreen dimensions
        viewableWidth = screenSize.width;
        viewableHeight = screenSize.height;
        scaleX = viewableWidth / (float)YE_STATE.engine.target_camera->camera->view_field.w;
        scaleY = viewableHeight / (float)YE_STATE.engine.target_camera->camera->view_field.h;

        // printf("Viewable size: %d, %d\n", viewableWidth, viewableHeight);

        *x = ((*x / scaleX) + campos.x);
        *y = ((*y / scaleY) + campos.y);
    } else {
        // if we are using letterboxing, use the letterbox dimensions
        viewableWidth = YE_STATE.engine.letterbox.w;
        viewableHeight = YE_STATE.engine.letterbox.h;
        scaleX = viewableWidth / (float)YE_STATE.engine.target_camera->camera->view_field.w;
        scaleY = viewableHeight / (float)YE_STATE.engine.target_camera->camera->view_field.h;

        *x = (((*x - YE_STATE.engine.letterbox.x) / scaleX) + campos.x);
        *y = (((*y - YE_STATE.engine.letterbox.y) / scaleY) + campos.y);
    }
}

bool ye_component_exists(struct ye_entity *entity, enum ye_component_type type){
    switch(type){
        case YE_COMPONENT_TRANSFORM:
            return entity->transform != NULL;
        case YE_COMPONENT_RENDERER:
            return entity->renderer != NULL;
        case YE_COMPONENT_CAMERA:
            return entity->camera != NULL;
        case YE_COMPONENT_PHYSICS:
            return entity->physics != NULL;
        case YE_COMPONENT_TAG:
            return entity->tag != NULL;
        case YE_COMPONENT_COLLIDER:
            return entity->collider != NULL;
        case YE_COMPONENT_LUA_SCRIPT:
            return entity->lua_script != NULL;
        case YE_COMPONENT_AUDIOSOURCE:
            return entity->audiosource != NULL;
        case YE_COMPONENT_BUTTON:
            return entity->button != NULL;
        default:
            return false;
    }
}

bool ye_draw_thick_line(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, int thickness, SDL_Color color) {
    // Calculate the angle of the line
    float angle = atan2(y2 - y1, x2 - x1);

    // Calculate the perpendicular offset for vertices
    float offsetX = sin(angle) * thickness / 2;
    float offsetY = cos(angle) * thickness / 2;

    // Define vertices for the line
    SDL_Vertex vertices[4];
    vertices[0] = (SDL_Vertex){{x1 + offsetX, y1 - offsetY},color};
    vertices[1] = (SDL_Vertex){{x1 - offsetX, y1 + offsetY},color};
    vertices[2] = (SDL_Vertex){{x2 + offsetX, y2 - offsetY},color};
    vertices[3] = (SDL_Vertex){{x2 - offsetX, y2 + offsetY},color};

    // Define indices for the line segments
    int indices[6] = {0, 1, 2, 1, 2, 3};

    // Render the geometry
    return SDL_RenderGeometry(renderer, NULL, vertices, 4, indices, 6);
}

void ye_draw_thick_rect(SDL_Renderer *renderer, float x, float y, float w, float h, int thickness, SDL_Color color){
    // normalize for negative values (editor selection can have negative w,h)
    if (w < 0) { x += w; w = -w; }
    if (h < 0) { y += h; h = -h; }

    // get half the thickness of the line, to calculate the adjusted draw positions
    float half_thickness = thickness / 2.0f;
    
    // draw top
    ye_draw_thick_line(renderer, x - half_thickness, y, x + w + half_thickness, y, thickness, color);
    // draw right
    ye_draw_thick_line(renderer, x + w, y - half_thickness, x + w, y + h + half_thickness, thickness, color);
    // draw bottom
    ye_draw_thick_line(renderer, x - half_thickness, y + h, x + w + half_thickness, y + h, thickness, color);
    // draw left
    ye_draw_thick_line(renderer, x, y - half_thickness, x, y + h + half_thickness, thickness, color);
}