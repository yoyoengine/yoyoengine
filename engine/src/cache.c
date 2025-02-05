/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <string.h>

#include <jansson.h>

#include <yoyoengine/yep.h>
#include <yoyoengine/json.h>
#include <yoyoengine/cache.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/logging.h>
#include <yoyoengine/ecs/renderer.h>

/*
    Head nodes for our lists tracking cached resources
*/
struct ye_texture_node * cached_textures_head;
struct ye_font_node * cached_fonts_head;
struct ye_color_node * cached_colors_head;

/*
    TODO: properly error check and validate every field
*/
void ye_pre_cache_scene(json_t *scene){
    if(scene == NULL){ // this should quite literally never be hit.
        ye_logf(error,"%s","Invalid scene passed to pre_cache_scene.\n");
        return;
    }

    // iterate through all objects in "entities", check if they have a "renderer" field,
    // if so, check "impl"->"src" and load it as an image
    json_t *entities = NULL;
    ye_json_array(scene,"entities",&entities);
    if(entities == NULL){
        ye_logf(error,"%s","Failed to read entities from scene.\n");
        return;
    }
    for(size_t i = 0; i < json_array_size(entities); i++){
        json_t *entity = NULL;      ye_json_arr_object(entities,i,&entity);

        // ye_json_log(entity);

        // get components obj
        json_t *components = NULL;  ye_json_object(entity,"components",&components);
        if(components == NULL){
            continue;
        }

        // skip if obj has no renderer component
        if(!ye_json_has_key(components,"renderer")){
            continue;
        }

        json_t *renderer = NULL;    ye_json_object(components,"renderer",&renderer);
        if(renderer == NULL){
            continue;
        }

        // get the type of renderer
        int type_int;
        if(!ye_json_int(renderer,"type",&type_int)) {
            return;
        }

        enum ye_component_renderer_type type = (enum ye_component_renderer_type)type_int;        

        json_t *impl = NULL;
        ye_json_object(renderer,"impl",&impl);
        if(impl == NULL){
            continue;
        }

        const char *src = NULL; // comply with mingw & clang
        // const char *path = NULL; // comply with mingw & clang
        switch(type){
            case YE_RENDERER_TYPE_IMAGE:
                if(!ye_json_string(impl,"src",&src)){
                    continue;
                }
                ye_image(src);
                break;
            case YE_RENDERER_TYPE_ANIMATION:
                // we are just gonna let the animation add cache this. so we dont have to extract more nested keys from the anim meta
                // // cache the master map
                // if(!ye_json_string(impl,"animation path",&path)){
                //     continue;
                // }
                // ye_image(path);

                // leaving this garbage for now because I anticipate doing an
                // animation rewrite soon. TODO
                break;
            default:
                break;
        }
    }
}

void ye_pre_cache_styles(const char *styles_path){
    if(styles_path == NULL){
        ye_logf(debug,"%s","No styles path provided.\n");
        return;
    }

    /*
        If we are in editor mode this file exists in local dir, else its packed
    */
    json_t *STYLES = NULL;
    // read fonts and colors from styles and cache them
    if(YE_STATE.editor.editor_mode){
        STYLES = ye_json_read(ye_path_resources(styles_path));
    }
    else{
        STYLES = yep_resource_json(styles_path);
    }

    // ye_json_log(STYLES);
    if(STYLES == NULL){
        ye_logf(error,"Failed to read styles file: %s\n",styles_path);
        json_decref(STYLES);
        return;
    }

    // check that fonts and colors exists and extract them if so
    if(ye_json_has_key(STYLES,"fonts")){
        json_t *fonts = NULL;
        ye_json_object(STYLES,"fonts",&fonts);
        if(fonts == NULL){
            ye_logf(error,"%s","Failed to read fonts from styles file.\n");
            json_decref(STYLES);
            return;
        }

        // iterate over fonts and cache them
        const char *font_name;
        json_t *font;
        json_object_foreach(fonts, font_name, font) {
            if(!ye_json_has_key(font,"path")){
                ye_logf(error,"Font %s does not have a path.\n",font_name);
                continue;
            }
            /*
            if(!ye_json_has_key(font,"size")){
                ye_logf(error,"Font %s does not have a size.\n",font_name);
                continue;
            }
            no more font sizes!
            */
            const char *font_path;    ye_json_string(font,"path",&font_path);
            // int font_size;      ye_json_int(font,"size",&font_size); no more font sizes!
            ye_cache_font(font_name,/*font_size,*/font_path); // path is really the handle (which is the realtive path from resources/)
        }
    }

    if(ye_json_has_key(STYLES,"colors")){
        json_t *colors = NULL;
        ye_json_object(STYLES,"colors",&colors);
        if(colors == NULL){
            ye_logf(error,"%s","Failed to read colors from styles file.\n");
            json_decref(STYLES);
            return;
        }

        // iterate over colors and cache them
        const char *color_name;
        json_t *color;
        json_object_foreach(colors, color_name, color) {
            if(!ye_json_has_key(color,"r")){
                ye_logf(error,"Color %s does not have an r value.\n",color_name);
                continue;
            }
            if(!ye_json_has_key(color,"g")){
                ye_logf(error,"Color %s does not have a g value.\n",color_name);
                continue;
            }
            if(!ye_json_has_key(color,"b")){
                ye_logf(error,"Color %s does not have a b value.\n",color_name);
                continue;
            }
            if(!ye_json_has_key(color,"a")){
                ye_logf(error,"Color %s does not have an a value.\n",color_name);
                continue;
            }
            int r;  ye_json_int(color,"r",&r);
            int g;  ye_json_int(color,"g",&g);
            int b;  ye_json_int(color,"b",&b);
            int a;  ye_json_int(color,"a",&a);
            SDL_Color sdl_color = {r,g,b,a};
            ye_cache_color(color_name,sdl_color);
        }
    }

    // free styles json
    json_decref(STYLES);
}

void ye_init_cache(){
    cached_textures_head = NULL;
    cached_fonts_head = NULL;
    cached_colors_head = NULL;
}

void ye_clear_texture_cache(){
    // free cached textures
    struct ye_texture_node *texture_node, *texture_tmp;
    HASH_ITER(hh, cached_textures_head, texture_node, texture_tmp) {
        HASH_DEL(cached_textures_head, texture_node);
        SDL_DestroyTexture(texture_node->texture);
        free(texture_node->path);
        free(texture_node);
    }
}

void ye_clear_font_cache(){
    // free cached fonts
    struct ye_font_node *font_node, *font_tmp;
    HASH_ITER(hh, cached_fonts_head, font_node, font_tmp) {
        HASH_DEL(cached_fonts_head, font_node);
        
        // make sure we dont clear the engine font if we had a failure loading this font from disk
        if(font_node->font != NULL && font_node->font != YE_STATE.engine.pEngineFont){
            // printf("Closing font: %s\n",font_node->name);
            TTF_CloseFont(font_node->font);
        }
        
        free(font_node->name);
        free(font_node);
    }
}

void ye_clear_color_cache(){
    // free cached colors
    struct ye_color_node *color_node, *color_tmp;
    HASH_ITER(hh, cached_colors_head, color_node, color_tmp) {
        HASH_DEL(cached_colors_head, color_node);
        free(color_node->name);
        free(color_node);
    }
}

void ye_shutdown_cache(){
    // free cached textures
    ye_clear_texture_cache();

    // free cached fonts
    ye_clear_font_cache();

    // free cached colors
    ye_clear_color_cache();

    ye_logf(info,"%s","Shut down cache.\n");
}

/*
    PRIMARY API:
    This is the intended interface with the cache system, but assumes you have pre cached fonts and colors.
*/

SDL_Texture * ye_image(const char *path){
    // check cache for texture named by path
    struct ye_texture_node *node = cached_textures_head;
    HASH_FIND_STR(cached_textures_head, path, node);
    if(node != NULL){
        // ye_logf(debug,"CACHE HIT: %s\n",path);
        return node->texture;
    }

    // if not found, load texture and add to cache
    // ye_logf(warning,"CACHE MISS: %s\n",path);
    return ye_cache_texture(path);
}

TTF_Font * ye_font(const char *name, int size){
    // check cache for font named by name and size
    struct ye_font_node *node;
    for(node = cached_fonts_head; node != NULL; node = node->hh.next) {
        if(strcmp(node->name, name) == 0) {
            // ye_logf(debug,"CACHE HIT: %s\n",name);

            // we found it, so resize if necessary
            if(node->size != size){
                // its good to resize here even if its larger, because huge fonts take much longer to render
                TTF_SetFontSize(node->font,size);
                node->size = size;
            }

            return node->font;
        }
    }

    ye_logf(error,"Font cache miss: %s. Returning default.\n",name);
    return YE_STATE.engine.pEngineFont;
}

SDL_Color * ye_color(const char *name){
    // check cache for color named by name
    struct ye_color_node *node;
    for(node = cached_colors_head; node != NULL; node = node->hh.next) {
        if(strcmp(node->name, name) == 0) {
            // ye_logf(debug,"CACHE HIT: %s\n",name);
            return &node->color;
        }
    }

    ye_logf(error,"Color cache miss: %s. Returning default.\n",name);
    return YE_STATE.engine.pEngineFontColor;
}

/*
    EXTENDED API:
    This is used by the primary API but can also be used directly by the developer.
*/

void ye_cache_texture_manual(SDL_Texture *texture, const char *key){
    // cache the texture
    struct ye_texture_node *new_node = malloc(sizeof(struct ye_texture_node));
    new_node->texture = texture;
    new_node->path = malloc(strlen(key) + 1);
    strcpy(new_node->path, key);
    HASH_ADD_KEYPTR(hh, cached_textures_head, new_node->path, strlen(new_node->path), new_node);
}

SDL_Texture * ye_cache_texture(const char *path){
    SDL_Texture *texture;
    SDL_Surface *sur = NULL;

    // try to get the surface from resources.yep if we arent in editor mode
    if(!YE_STATE.editor.editor_mode)
        sur = yep_resource_image(path);

    // if we didnt find it, load it from disk, else create texture from it
    if(sur == NULL){
        texture = ye_create_image_texture(ye_path_resources(path));
    }
    else{
        texture = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, sur);
        SDL_DestroySurface(sur);
    }

    // cache the texture
    ye_cache_texture_manual(texture, path);

    // struct ye_texture_node *new_node = malloc(sizeof(struct ye_texture_node));
    // new_node->texture = texture;
    // new_node->path = malloc(strlen(path) + 1);
    // strcpy(new_node->path, path);
    // HASH_ADD_KEYPTR(hh, cached_textures_head, new_node->path, strlen(new_node->path), new_node);
    // ye_logf(debug,"Cached texture: %s\n",path);
    return texture;
}

TTF_Font * ye_cache_font_manual(const char *name, TTF_Font *font){
    // cache the font
    struct ye_font_node *new_node = malloc(sizeof(struct ye_font_node));
    new_node->font = font;
    new_node->name = malloc(strlen(name) + 1);
    strcpy(new_node->name, name);
    new_node->size = 1; // we load the fonts at size 1 for now
    HASH_ADD_KEYPTR(hh, cached_fonts_head, new_node->name, strlen(new_node->name), new_node);
    // ye_logf(debug,"Cached font: %s\n",name);
    return font;
}

TTF_Font * ye_cache_font(const char *name, /*int size,*/ const char *path){
    TTF_Font *font = NULL;

    // try to get the font from resources.yep if we arent in editor mode
    if(!YE_STATE.editor.editor_mode)
        font = yep_resource_font(path);

    // if we didnt find it, load it from disk
    if(font == NULL){
        font = ye_load_font(ye_path_resources(path)/*, size*/);
    }

    // cache the font
    struct ye_font_node *new_node = malloc(sizeof(struct ye_font_node));
    new_node->font = font;
    new_node->name = malloc(strlen(name) + 1);
    strcpy(new_node->name, name);
    new_node->size = 1; // we load the fonts at size 1 for now
    HASH_ADD_KEYPTR(hh, cached_fonts_head, new_node->name, strlen(new_node->name), new_node);
    // ye_logf(debug,"Cached font: %s\n",name);
    return font;
}

SDL_Color * ye_cache_color(const char *name, SDL_Color color){
    // cache the color
    struct ye_color_node *new_node = malloc(sizeof(struct ye_color_node));
    new_node->color = color;
    new_node->name = malloc(strlen(name) + 1);
    strcpy(new_node->name, name);
    HASH_ADD_KEYPTR(hh, cached_colors_head, new_node->name, strlen(new_node->name), new_node);
    // ye_logf(debug,"Cached color: %s\n",name);
    return &new_node->color;
}