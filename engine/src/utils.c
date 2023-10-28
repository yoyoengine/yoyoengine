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

#include <yoyoengine/yoyoengine.h>

/*
    Massive function to handle the orientation of an object within a set of bounds
*/
// void (SDL_Rect* bounds, SDL_Rect* obj, Alignment alignment, SDL_Point* center){
//     // check if some loser wants to stretch something
//     if(alignment == ALIGN_STRETCH){
//         obj->w = bounds->w;
//         obj->h = bounds->h;
//         obj->x = bounds->x;
//         obj->y = bounds->y;
//         return;
//     }

//     // actual orientation handling

//     float boundsAspectRatio = (float)bounds->w / (float)bounds->h;
//     float objectAspectRatio = (float)obj->w / (float)obj->h;

//     if (objectAspectRatio > boundsAspectRatio) {
//         obj->h = bounds->w / objectAspectRatio;
//         obj->w = bounds->w;
//     } else {
//         obj->w = bounds->h * objectAspectRatio;
//         obj->h = bounds->h;
//     }

//     switch(alignment) {
//         case ALIGN_TOP_LEFT:
//             obj->x = bounds->x;
//             obj->y = bounds->y;
//             break;
//         case ALIGN_TOP_CENTER:
//             obj->x = bounds->x + (bounds->w - obj->w) / 2;
//             obj->y = bounds->y;
//             break;
//         case ALIGN_TOP_RIGHT:
//             obj->x = bounds->x + (bounds->w - obj->w);
//             obj->y = bounds->y;
//             break;
//         case ALIGN_MID_LEFT:
//             obj->x = bounds->x;
//             obj->y = bounds->y + (bounds->h - obj->h) / 2;
//             break;
//         case ALIGN_MID_CENTER:
//             obj->x = bounds->x + (bounds->w - obj->w) / 2;
//             obj->y = bounds->y + (bounds->h - obj->h) / 2;
//             break;
//         case ALIGN_MID_RIGHT:
//             obj->x = bounds->x + (bounds->w - obj->w);
//             obj->y = bounds->y + (bounds->h - obj->h) / 2;
//             break;
//         case ALIGN_BOT_LEFT:
//             obj->x = bounds->x;
//             obj->y = bounds->y + (bounds->h - obj->h);
//             break;
//         case ALIGN_BOT_CENTER:
//             obj->x = bounds->x + (bounds->w - obj->w) / 2;
//             obj->y = bounds->y + (bounds->h - obj->h);
//             break;
//         case ALIGN_BOT_RIGHT:
//             obj->x = bounds->x + (bounds->w - obj->w);
//             obj->y = bounds->y + (bounds->h - obj->h);
//             break;
//         default:
//             ye_logf(error, "Invalid alignment\n");
//             break;
//     }
//     *center = (SDL_Point){obj->w / 2, obj->h / 2};
// }

/*
    Slow (probably) function that jumps through a bunch of hoops aligning struct rectf's
    It has to convert them to SDL_Rect's and back because the SDL_Rect's are used for rendering
    but could probably be refactored because its just floating point math anyways

    TODO: SDL_Point should be floating too
*/
void ye_auto_fit_bounds(struct ye_rectf* bounds_f, struct ye_rectf* obj_f, enum ye_alignment alignment, SDL_Point* center){
    SDL_Rect _bounds = ye_convert_rectf_rect(*bounds_f);
    SDL_Rect _obj = ye_convert_rectf_rect(*obj_f);
    SDL_Rect * bounds = &_bounds;
    SDL_Rect * obj = &_obj;

    // check if some loser wants to stretch something
    if(alignment == YE_ALIGN_STRETCH){
        obj->w = bounds->w;
        obj->h = bounds->h;
        obj->x = bounds->x;
        obj->y = bounds->y;
        return;
    }

    // actual orientation handling

    float boundsAspectRatio = (float)bounds->w / (float)bounds->h;
    float objectAspectRatio = (float)obj->w / (float)obj->h;

    if (objectAspectRatio > boundsAspectRatio) {
        obj->h = bounds->w / objectAspectRatio;
        obj->w = bounds->w;
    } else {
        obj->w = bounds->h * objectAspectRatio;
        obj->h = bounds->h;
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

/*
    Get a rect of the pixel w,h of a texture
*/
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
    Given two x,y points, one being source one being reciever, calculate the angle (north is 0) between the reciever and the source
*/
float ye_get_angle(float x1, float y1, float x2, float y2){
    float angle = atan2(y2 - y1, x2 - x1) * 180 / M_PI;
    if(angle < 0){
        angle += 360;
    }
    return angle;
}

/*
    Takes in an entity and a component type and returns the position of that component
    (sending transform will get you just the transform, a specific component will get
    its actual position, factoring in relativity)
*/
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

                // if relative adjust its position
                if(entity->collider->relative && entity->transform != NULL){
                    pos.x += entity->transform->x;
                    pos.y += entity->transform->y;
                }

                return pos;
            }
            ye_logf(error,"Tried to get position of a null collider component on entity \"%s\". returning (0,0,0,0)\n",entity->name);
            return pos;
        default:
            ye_logf(error, "Tried to get position for component on \"%s\" that does not have a position or size. returning (0,0,0,0)\n",entity->name);
            return pos;
    }
}

/*
    Wrapper for ye_get_position(), returning a SDL_Rect of integers
*/
SDL_Rect ye_get_position_rect(struct ye_entity *entity, enum ye_component_type type){
    struct ye_rectf pos = ye_get_position(entity, type);
    SDL_Rect rect = {pos.x, pos.y, pos.w, pos.h};
    return rect;
}