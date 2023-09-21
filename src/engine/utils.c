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
    if(alignment == ALIGN_STRETCH){
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
        case ALIGN_TOP_LEFT:
            obj->x = bounds->x;
            obj->y = bounds->y;
            break;
        case ALIGN_TOP_CENTER:
            obj->x = bounds->x + (bounds->w - obj->w) / 2;
            obj->y = bounds->y;
            break;
        case ALIGN_TOP_RIGHT:
            obj->x = bounds->x + (bounds->w - obj->w);
            obj->y = bounds->y;
            break;
        case ALIGN_MID_LEFT:
            obj->x = bounds->x;
            obj->y = bounds->y + (bounds->h - obj->h) / 2;
            break;
        case ALIGN_MID_CENTER:
            obj->x = bounds->x + (bounds->w - obj->w) / 2;
            obj->y = bounds->y + (bounds->h - obj->h) / 2;
            break;
        case ALIGN_MID_RIGHT:
            obj->x = bounds->x + (bounds->w - obj->w);
            obj->y = bounds->y + (bounds->h - obj->h) / 2;
            break;
        case ALIGN_BOT_LEFT:
            obj->x = bounds->x;
            obj->y = bounds->y + (bounds->h - obj->h);
            break;
        case ALIGN_BOT_CENTER:
            obj->x = bounds->x + (bounds->w - obj->w) / 2;
            obj->y = bounds->y + (bounds->h - obj->h);
            break;
        case ALIGN_BOT_RIGHT:
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