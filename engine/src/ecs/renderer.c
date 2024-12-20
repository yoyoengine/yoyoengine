/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <string.h>

#include <SDL_ttf.h>
#include <jansson.h>

#include <yoyoengine/yep.h>
#include <yoyoengine/json.h>
#include <yoyoengine/cache.h>
#include <yoyoengine/event.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/graphics.h>
#include <yoyoengine/version.h>
#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/ecs/camera.h>
#include <yoyoengine/ecs/transform.h>
#include <yoyoengine/ecs/renderer.h>
#include <yoyoengine/ecs/collider.h>
#include <yoyoengine/debug_renderer.h>

#include <yoyoengine/tar_physics/solver.h>

#include <yoyoengine/types.h>

void ye_update_renderer_component(struct ye_entity *entity){
    /*The purpose of this function is to be invoked when we know we have changed some internal variables of the renderer, and need to recompute the outputted texture*/
    switch(entity->renderer->type){
        case YE_RENDERER_TYPE_IMAGE:
            entity->renderer->texture = ye_image(
                entity->renderer->renderer_impl.image->src
            );
            break;
        case YE_RENDERER_TYPE_TEXT:
            // destroy old text texture (not managed in cache)
            SDL_DestroyTexture(entity->renderer->texture);

            // fetch new colors and fonts from cache
            entity->renderer->renderer_impl.text->font = ye_font(entity->renderer->renderer_impl.text->font_name, entity->renderer->renderer_impl.text->font_size);
            entity->renderer->renderer_impl.text->color = ye_color(entity->renderer->renderer_impl.text->color_name);

            // create new text texture
            if(entity->renderer->renderer_impl.text->wrap_width > 0)
                entity->renderer->texture = createTextTextureWrapped(entity->renderer->renderer_impl.text->text, entity->renderer->renderer_impl.text->font, entity->renderer->renderer_impl.text->color, entity->renderer->renderer_impl.text->wrap_width);
            else
                entity->renderer->texture = createTextTexture(entity->renderer->renderer_impl.text->text, entity->renderer->renderer_impl.text->font, entity->renderer->renderer_impl.text->color);
            break;
        case YE_RENDERER_TYPE_TEXT_OUTLINED:
            // destroy old text texture (not managed in cache)
            SDL_DestroyTexture(entity->renderer->texture);

            // fetch new colors and fonts from cache
            entity->renderer->renderer_impl.text_outlined->font = ye_font(entity->renderer->renderer_impl.text_outlined->font_name, entity->renderer->renderer_impl.text_outlined->font_size);
            entity->renderer->renderer_impl.text_outlined->color = ye_color(entity->renderer->renderer_impl.text_outlined->color_name);
            entity->renderer->renderer_impl.text_outlined->outline_color = ye_color(entity->renderer->renderer_impl.text_outlined->outline_color_name);

            // create new text texture
            if(entity->renderer->renderer_impl.text_outlined->wrap_width > 0)
                entity->renderer->texture = createTextTextureWithOutlineWrapped(entity->renderer->renderer_impl.text_outlined->text, entity->renderer->renderer_impl.text_outlined->outline_size, entity->renderer->renderer_impl.text_outlined->font, entity->renderer->renderer_impl.text_outlined->color, entity->renderer->renderer_impl.text_outlined->outline_color, entity->renderer->renderer_impl.text_outlined->wrap_width);
            else
                entity->renderer->texture = createTextTextureWithOutline(entity->renderer->renderer_impl.text_outlined->text, entity->renderer->renderer_impl.text_outlined->outline_size, entity->renderer->renderer_impl.text_outlined->font, entity->renderer->renderer_impl.text_outlined->color, entity->renderer->renderer_impl.text_outlined->outline_color);
            break;
        case YE_RENDERER_TYPE_TILEMAP_TILE:
            entity->renderer->texture = ye_image(
                entity->renderer->renderer_impl.tile->handle
            );
            break;
        default: ; // this semicolon fixes a mingw complaint
            // try to open new meta file and get out "src" field
            json_t *META = NULL;
            if(YE_STATE.editor.editor_mode)
                META = ye_json_read(ye_path_resources(entity->renderer->renderer_impl.animation->meta_file));
            else
                META = yep_resource_json(entity->renderer->renderer_impl.animation->meta_file);
            
            if(META == NULL){
                ye_logf(error, "Failed to load animation meta file %s\n", entity->renderer->renderer_impl.animation->meta_file);
                return;
            }

            // get src
            const char *path = NULL;
            if(!ye_json_string(META, "src", &path)){
                ye_logf(error, "Failed to load SRC from animation meta file %s\n", entity->renderer->renderer_impl.animation->meta_file);
                json_decref(META);
                return;
            }

            // attempt to open src, if we get an error then it does not exist
            SDL_Texture *texture = ye_image(path);

            if(texture == NULL){
                ye_logf(error, "Failed to load animation texture file %s\n", path);
                json_decref(META);
                return;
            }

            // if we made it here, the proposed change exists, so delete and re add the animator component with the same z
            char *meta_file = strdup(entity->renderer->renderer_impl.animation->meta_file);
            int z = entity->renderer->z;

            ye_remove_renderer_component(entity);
            ye_add_animation_renderer_component(entity, z, meta_file);

            json_decref(META);
            free(meta_file);
            break;
    }
}

void ye_add_renderer_component(
    struct ye_entity *entity, 
    enum ye_component_renderer_type type, 
    int z, 
    // struct ye_rectf rect, 
    void *data
    ){

    entity->renderer = malloc(sizeof(struct ye_component_renderer));
    entity->renderer->active = true;
    entity->renderer->type = type;
    entity->renderer->alpha = 255; // by default renderer is fully opaque
    entity->renderer->z = z;
    // entity->renderer->rect = rect;
    
    // must be modified outside of this constructor if non default desired
    entity->renderer->rect = (struct ye_rectf){0,0,0,0};
    entity->renderer->rotation = 0;
    entity->renderer->flipped_x = false;
    entity->renderer->flipped_y = false;
    entity->renderer->center = (SDL_Point){0, 0};       // default center is the center of the bounds
    entity->renderer->alignment = YE_ALIGN_MID_CENTER;  // default alignment is mid center
    entity->renderer->preserve_original_size = false;   // default is to grow to fit
    entity->renderer->relative = true;                  // default is relative positioning

    if(type == YE_RENDERER_TYPE_IMAGE){
        entity->renderer->renderer_impl.image = data;
    }
    else if(type == YE_RENDERER_TYPE_TEXT){
        entity->renderer->renderer_impl.text = data;
    }
    else if(type == YE_RENDERER_TYPE_TEXT_OUTLINED){
        entity->renderer->renderer_impl.text_outlined = data;
    }
    else if(type == YE_RENDERER_TYPE_ANIMATION){
        entity->renderer->renderer_impl.animation = data;
    }
    else if(type == YE_RENDERER_TYPE_TILEMAP_TILE){
        entity->renderer->renderer_impl.tile = data;
    }
    else{
        ye_logf(error, "Attempt add Invalid renderer type %d\n", type);
    }

    // add this entity to the renderer component list
    ye_entity_list_add_sorted_renderer_z(&renderer_list_head, entity);

    // log that we added a renderer and to what ID
    // ye_logf(debug, "Added renderer to entity %d\n", entity->id);
}

void ye_add_image_renderer_component(struct ye_entity *entity, int z, const char *src){
    struct ye_component_renderer_image *image = malloc(sizeof(struct ye_component_renderer_image));
    // copy src to image->src
    image->src = malloc(sizeof(char) * (strlen(src) + 1));
    strcpy(image->src, src);

    // create the renderer top level
    ye_add_renderer_component(entity, YE_RENDERER_TYPE_IMAGE, z, image);

    // create the image texture
    entity->renderer->texture = ye_image(src);

    // update rect based off generated image
    SDL_Rect size = ye_get_real_texture_size_rect(entity->renderer->texture);
    entity->renderer->rect.w = size.w;
    entity->renderer->rect.h = size.h;

    // default center to be the middle of the rect
    entity->renderer->center = (SDL_Point){entity->renderer->rect.w / 2, entity->renderer->rect.h / 2};
}

void ye_add_image_renderer_component_preloaded(struct ye_entity *entity, int z, SDL_Texture *texture){
    struct ye_component_renderer_image *image = malloc(sizeof(struct ye_component_renderer_image));
    image->src = NULL;

    // create the renderer top level
    ye_add_renderer_component(entity, YE_RENDERER_TYPE_IMAGE, z, image);

    // set the texture
    entity->renderer->texture = texture;

    // update rect based off generated image
    SDL_Rect size = ye_get_real_texture_size_rect(entity->renderer->texture);
    entity->renderer->rect.w = size.w;
    entity->renderer->rect.h = size.h;
}

void ye_add_text_renderer_component(struct ye_entity *entity, int z, const char *text, const char* font, int font_size, const char *color, int wrap_width){
    struct ye_component_renderer_text *text_renderer = malloc(sizeof(struct ye_component_renderer_text));
    text_renderer->text = strdup(text);

    text_renderer->font = ye_font(font, font_size);
    text_renderer->font_name = strdup(font);
    text_renderer->font_size = font_size;

    text_renderer->wrap_width = wrap_width;

    text_renderer->color = ye_color(color);
    text_renderer->color_name = strdup(color);

    // create the renderer top level
    ye_add_renderer_component(entity, YE_RENDERER_TYPE_TEXT, z, text_renderer);

    // create the text texture
    if(wrap_width > 0){
        entity->renderer->texture = createTextTextureWrapped(text, text_renderer->font, text_renderer->color, wrap_width);
    }
    else{
        entity->renderer->texture = createTextTexture(text, text_renderer->font, text_renderer->color);
    }

    // update rect based off generated image
    SDL_Rect size = ye_get_real_texture_size_rect(entity->renderer->texture);
    entity->renderer->rect.w = size.w;
    entity->renderer->rect.h = size.h;
}

void ye_add_text_outlined_renderer_component(struct ye_entity *entity, int z, const char *text, const char *font, int font_size, const char *color, const char *outline_color, int outline_size, int wrap_width){
    struct ye_component_renderer_text_outlined *text_renderer = malloc(sizeof(struct ye_component_renderer_text_outlined));
    text_renderer->text = strdup(text);

    text_renderer->font = ye_font(font, font_size);
    text_renderer->font_name = strdup(font);
    text_renderer->font_size = font_size;

    text_renderer->wrap_width = wrap_width;

    text_renderer->color = ye_color(color);
    text_renderer->color_name = strdup(color);

    text_renderer->outline_color = ye_color(outline_color);
    text_renderer->outline_color_name = strdup(outline_color);

    text_renderer->outline_size = outline_size;

    // create the renderer top level
    ye_add_renderer_component(entity, YE_RENDERER_TYPE_TEXT_OUTLINED, z, text_renderer);

    // create the text texture
    if(wrap_width > 0)
        entity->renderer->texture = createTextTextureWithOutlineWrapped(text,outline_size,text_renderer->font,text_renderer->color,text_renderer->outline_color,wrap_width);
    else
        entity->renderer->texture = createTextTextureWithOutline(text,outline_size,text_renderer->font,text_renderer->color,text_renderer->outline_color);

    // update rect based off generated image
    SDL_Rect size = ye_get_real_texture_size_rect(entity->renderer->texture);
    entity->renderer->rect.w = size.w;
    entity->renderer->rect.h = size.h;
}

void ye_add_animation_renderer_component(struct ye_entity *entity, int z, const char *meta_file){
    // load the meta file
    json_t *META = NULL;
    if(YE_STATE.editor.editor_mode)
        META = ye_json_read(ye_path_resources(meta_file));
    else
        META = yep_resource_json(meta_file);
    
    if(META == NULL){
        ye_logf(error, "Failed to load animation meta file %s\n", meta_file);
        entity->renderer = NULL; // just in case :P
        return;
    }

    // version of the file
    int version; ye_json_int(META, "version", &version);
    if(version != YOYO_ENGINE_ANIMATION_FILE_VERSION){
        ye_logf(error, "Invalid animation meta file version %d against %d\n", version, YOYO_ENGINE_ANIMATION_FILE_VERSION);
        json_decref(META);
        return;
    }

    // source location of the map    
    const char *path = NULL; 
    if(!ye_json_string(META, "src", &path)){
        ye_logf(error, "Failed to load SRC from animation meta file %s\n", meta_file);
        json_decref(META);
        return;
    }

    // size of each frame
    int frame_width;
    if(!ye_json_int(META, "frame_width", &frame_width)){
        ye_logf(error, "Failed to load frame_width from animation meta file %s\n", meta_file);
        json_decref(META);
        return;
    }
    int frame_height;
    if(!ye_json_int(META, "frame_height", &frame_height)){
        ye_logf(error, "Failed to load frame_height from animation meta file %s\n", meta_file);
        json_decref(META);
        return;
    }

    // number of frames
    int frame_count;
    if(!ye_json_int(META, "frame_count", &frame_count)){
        ye_logf(error, "Failed to load frame_count from animation meta file %s\n", meta_file);
        json_decref(META);
        return;
    }

    // frame delay
    int frame_delay;
    if(!ye_json_int(META, "frame_delay", &frame_delay)){
        ye_logf(error, "Failed to load frame_delay from animation meta file %s\n", meta_file);
        json_decref(META);
        return;
    }

    // loops
    int loops;
    if(!ye_json_int(META, "loops", &loops)){
        ye_logf(error, "Failed to load loops from animation meta file %s\n", meta_file);
        json_decref(META);
        return;
    }

    struct ye_component_renderer_animation *animation = malloc(sizeof(struct ye_component_renderer_animation));
    animation->frame_count = frame_count;
    animation->frame_delay = frame_delay;
    animation->loops = loops;
    animation->last_updated = 0; // set as 0 now so the operations between now and setting it do not count towards its frame time
    animation->current_frame_index = 0;
    animation->paused = false;
    animation->animation_handle = strdup(path);
    animation->meta_file = strdup(meta_file);
    animation->frame_width = frame_width;
    animation->frame_height = frame_height;

    // create the renderer top level
    ye_add_renderer_component(entity, YE_RENDERER_TYPE_ANIMATION, z, animation);

    // set the texture to the the map
    entity->renderer->texture = ye_image(path);

    // update rect based off of frame size
    entity->renderer->rect.w = frame_width;
    entity->renderer->rect.h = frame_height;

    animation->last_updated = SDL_GetTicks(); // set the last updated to now so we can start ticking it accurately

    // free the meta file
    json_decref(META);
}

void ye_add_tilemap_renderer_component(struct ye_entity *entity, int z, const char * handle, SDL_Rect src){
    struct ye_component_renderer_tilemap_tile *tile = malloc(sizeof(struct ye_component_renderer_tilemap_tile));
    tile->handle = strdup(handle);
    tile->src = src;

    // create the renderer top level
    ye_add_renderer_component(entity, YE_RENDERER_TYPE_TILEMAP_TILE, z, tile);

    // create the tile texture
    entity->renderer->texture = ye_image(handle);

    // update rect based off of src size
    entity->renderer->rect.w = src.w;
    entity->renderer->rect.h = src.h;
}

void ye_remove_renderer_component(struct ye_entity *entity){
    // free contents of renderer_impl
    switch(entity->renderer->type){
        case YE_RENDERER_TYPE_IMAGE:
            free(entity->renderer->renderer_impl.image->src);
            free(entity->renderer->renderer_impl.image);
            break;
        case YE_RENDERER_TYPE_TEXT:
            free(entity->renderer->renderer_impl.text->text);
            // free the strings we strdup'd before the impl itself (duh)
            free(entity->renderer->renderer_impl.text->font_name);
            free(entity->renderer->renderer_impl.text->color_name);
            free(entity->renderer->renderer_impl.text);

            // text textures are not stored in cache, manually remove them
            SDL_DestroyTexture(entity->renderer->texture);
            break;
        case YE_RENDERER_TYPE_TEXT_OUTLINED:
            free(entity->renderer->renderer_impl.text_outlined->text);
            // free the strings we strdup'd before the impl itself (duh)
            free(entity->renderer->renderer_impl.text_outlined->font_name);
            free(entity->renderer->renderer_impl.text_outlined->color_name);
            free(entity->renderer->renderer_impl.text_outlined->outline_color_name);
            free(entity->renderer->renderer_impl.text_outlined);

            // text textures are not stored in cache, manually remove them
            SDL_DestroyTexture(entity->renderer->texture);
            break;
        case YE_RENDERER_TYPE_ANIMATION:
            // cache will handle freeing the frame map as needed
            free(entity->renderer->renderer_impl.animation->animation_handle);
            free(entity->renderer->renderer_impl.animation->meta_file);
            free(entity->renderer->renderer_impl.animation);
            break;
        case YE_RENDERER_TYPE_TILEMAP_TILE:
            free(entity->renderer->renderer_impl.tile->handle);
            free(entity->renderer->renderer_impl.tile);
            break;
    }

    // cache will handle freeing the texture as needed

    free(entity->renderer);
    entity->renderer = NULL;

    // remove the entity from the renderer component list
    ye_entity_list_remove(&renderer_list_head, entity);
}

void _draw_subsecting_lines(SDL_Renderer * renderer, SDL_Rect cam, int line_spacing, int thickness, SDL_Color color) {
    int num_lines = cam.w / line_spacing;
    int x_offset = cam.x % line_spacing;

    for(int i = 0; i < num_lines; i++){
        ye_draw_thick_line(
            renderer,
            (i * line_spacing) - x_offset,
            0,
            (i * line_spacing) - x_offset,
            cam.h,
            thickness,
            color
        );
    }
    
    int y_offset = cam.y % line_spacing;

    for(int i = 0; i < num_lines; i++){
        ye_draw_thick_line(
            renderer,
            0,
            (i * line_spacing) - y_offset,
            cam.w,
            (i * line_spacing) - y_offset,
            thickness,
            color
        );
    }

    // printf("drew %dx%d lines\n", num_lines, num_lines);
    // printf("x offset: %d, y offset: %d\n", x_offset, y_offset);
}

void _paint_viewport_lines(SDL_Renderer *renderer) {
    // get the cam pos
    SDL_Rect cam = ye_get_position_rect(YE_STATE.engine.target_camera,YE_COMPONENT_CAMERA);

    /*
        Grid of subsecting lines
    */
    if(cam.w < 2560)
        _draw_subsecting_lines(renderer, cam, 50, 1, (SDL_Color){25, 25, 25, 255});
    // if(cam.w < 3840)
    if(cam.w < 5000)
        _draw_subsecting_lines(renderer, cam, 250, 3, (SDL_Color){50, 50, 50, 255});
    if(cam.w < 9500)
        _draw_subsecting_lines(renderer, cam, 500, 5, (SDL_Color){75, 75, 75, 255});
    if(cam.w > 10000){
        int thickness = ((cam.w - 9500) / 2000) + 7;
        // printf("thickness: %d\n", thickness);
        _draw_subsecting_lines(renderer, cam, 1500, thickness, (SDL_Color){100, 100, 100, 255});
    }
    else{
        _draw_subsecting_lines(renderer, cam, 1500, 5, (SDL_Color){100, 100, 100, 255});
    }

    /*
        Overpaint the axes
    */

    // x axis
    ye_draw_thick_line(
        renderer,
        0,
        0 - cam.y,
        cam.w,
        0 - cam.y,
        10,
        (SDL_Color){225, 225, 225, 255}
    );

    // y axis
    ye_draw_thick_line(
        renderer,
        0 - cam.x,
        0,
        0 - cam.x,
        cam.h,
        10,
        (SDL_Color){225, 225, 225, 255}
    );
}

void _attempt_tick_animation(struct ye_entity_node *current) {
    // TODO: this should be decoupled from the renderer and become its own system

    // if not editor mode (we want to not run animations in editor)
    if(!YE_STATE.editor.editor_mode){
        struct ye_component_renderer_animation *animation = current->entity->renderer->renderer_impl.animation;
        if(!animation->paused){
            int now = SDL_GetTicks();
            if(now - animation->last_updated >= animation->frame_delay){
                // the difference between now and last updated
                int diff = (now - animation->last_updated);// / (animation->frame_delay); 

                // the number of frames we need to advance
                int frames_to_advance = diff / animation->frame_delay;

                // advance the frame index and wrap around as needed
                animation->current_frame_index += frames_to_advance;
                if((size_t)animation->current_frame_index >= animation->frame_count){
                    animation->current_frame_index = animation->current_frame_index % animation->frame_count;
                    if(animation->loops != -1){
                        animation->loops--;
                        if(animation->loops <= 0){
                            animation->paused = true; // TODO: dont just pause when it ends, but give option to destroy/ disable renderer
                            // pause on the last frame of the animation
                            animation->current_frame_index = animation->frame_count - 1;
                        }
                    }
                }
                animation->last_updated = now;
                // current->entity->renderer->texture = animation->frames[animation->current_frame_index]; was this the only thing to change?
            }
        }
    }
}

/*
    Often, an renderer entity will request to be aligned in a bounding box.

    This function looks at the AABB of the renderer and it's bound and returns a transformation matrix
    to align the renderer in the bounding box, usually involing a scale and translation

    ex:
    [sx, 0,  tx]
    [0 , sy, ty]
    [0 , 0,  1 ]
*/
mat3_t _get_auto_bound(struct ye_rectf *bound_AABB, struct ye_rectf *child_AABB, enum ye_alignment alignment, bool should_grow_to_fit) {
    // Reset positions for calculation
    bound_AABB->x = 0;
    bound_AABB->y = 0;
    child_AABB->x = 0;
    child_AABB->y = 0;

    // Calculate scale first
    vec2_t scale = {.data={1.0f, 1.0f}};
    if (should_grow_to_fit || alignment == YE_ALIGN_STRETCH) {
        float child_w = child_AABB->w != 0 ? child_AABB->w : 1.0f;
        float child_h = child_AABB->h != 0 ? child_AABB->h : 1.0f;
        
        if (alignment != YE_ALIGN_STRETCH) {
            // Preserve aspect ratio using minimum scale
            float scale_x = bound_AABB->w / child_w;
            float scale_y = bound_AABB->h / child_h;
            float uniform_scale = fminf(scale_x, scale_y);
            scale = (vec2_t){.data={uniform_scale, uniform_scale}};
        } else {
            // Stretch to fill
            scale = (vec2_t){.data={bound_AABB->w / child_w, bound_AABB->h / child_h}};
        }
    }

    mat3_t transform = lla_mat3_identity();
    transform = lla_mat3_scale_vec2(transform, scale);

    // Calculate scaled dimensions
    float scaled_w = child_AABB->w * scale.data[0];
    float scaled_h = child_AABB->h * scale.data[1];
    
    // Calculate margins
    float margin_x = bound_AABB->w - scaled_w;
    float margin_y = bound_AABB->h - scaled_h;

    // Calculate translation based on alignment
    vec2_t translation = {.data={0, 0}};
    switch (alignment) {
        case YE_ALIGN_TOP_LEFT:
            break;
        case YE_ALIGN_TOP_CENTER:
            translation.data[0] = margin_x * 0.5f;
            break;
        case YE_ALIGN_TOP_RIGHT:
            translation.data[0] = margin_x;
            break;
        case YE_ALIGN_MID_LEFT:
            translation.data[1] = margin_y * 0.5f;
            break;
        case YE_ALIGN_MID_CENTER:
            translation.data[0] = margin_x * 0.5f;
            translation.data[1] = margin_y * 0.5f;
            break;
        case YE_ALIGN_MID_RIGHT:
            translation.data[0] = margin_x;
            translation.data[1] = margin_y * 0.5f;
            break;
        case YE_ALIGN_BOT_LEFT:
            translation.data[1] = margin_y;
            break;
        case YE_ALIGN_BOT_CENTER:
            translation.data[0] = margin_x * 0.5f;
            translation.data[1] = margin_y;
            break;
        case YE_ALIGN_BOT_RIGHT:
            translation.data[0] = margin_x;
            translation.data[1] = margin_y;
            break;
        case YE_ALIGN_STRETCH:
            break;
    }

    /*
        shoutout claude for this one. We have to revert the scale because even
        though there is no translation component it still skews the translation
    */
    translation.data[0] /= scale.data[0];
    translation.data[1] /= scale.data[1];

    transform = lla_mat3_translate(transform, translation);

    return transform;
}

// TODO: refactor for prect
void _paint_paintbounds(SDL_Renderer *renderer, struct ye_entity_node *current) {
    // avoid painting the editor origin TODO: reserve special name/id for editor entities since a user naming an entity origin will exclude them here...
    if (current->entity->name != NULL && strcmp(current->entity->name, "origin") == 0) {
        return;
    }
    
    // paint bounds, my beloved <3
    if (YE_STATE.editor.paintbounds_visible) {
        for(int i = 0; i < 4; i++){
            float x1 = current->entity->renderer->_cam_verts[i].position.x;
            float y1 = current->entity->renderer->_cam_verts[i].position.y;
            float x2 = current->entity->renderer->_cam_verts[(i + 1) % 4].position.x;
            float y2 = current->entity->renderer->_cam_verts[(i + 1) % 4].position.y;

            ye_draw_thick_line(renderer, x1, y1, x2, y2, 2, (SDL_Color){255, 0, 0, 255});
        }

        // TODO: paint a center marker, renderer or transform center? todo: integrate renderer center tighter and fix computation from tex size rather than rect size
    }

    // // button bounds
    // if(current->entity->button != NULL && YE_STATE.editor.button_bounds_visible){
    //     // paint the button bounds
    //     SDL_Rect button_bounds = ye_convert_rectf_rect(ye_get_position(current->entity,YE_COMPONENT_BUTTON));
    //     // printf("button_bounds: %d %d %d %d\n", button_bounds.x, button_bounds.y, button_bounds.w, button_bounds.h);
    //     button_bounds.x = button_bounds.x - camera_rect.x;
    //     button_bounds.y = button_bounds.y - camera_rect.y;
    //     SDL_SetRenderDrawColor(renderer, 235, 52, 235, 255);
    //     SDL_RenderDrawRect(renderer, &button_bounds);
    //     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // }

    // // audio range
    // if(current->entity->audiosource != NULL && YE_STATE.editor.audiorange_visible){
    //     SDL_Rect audio_range_rect = ye_convert_rectf_rect(
    //         ye_get_position(current->entity,YE_COMPONENT_AUDIOSOURCE)
    //     );
    //     audio_range_rect.x = audio_range_rect.x - camera_rect.x;
    //     audio_range_rect.y = audio_range_rect.y - camera_rect.y;
    //     SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    //     // use ye_draw_circle to draw the audio range, the width is the radius
    //     ye_draw_circle(renderer, audio_range_rect.x + (audio_range_rect.w / 2), audio_range_rect.y + (audio_range_rect.h / 2), audio_range_rect.w / 2, 5);

    //     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // }

    // if(YE_STATE.editor.editor_mode && YE_STATE.editor.display_names){
    //     // paint the entity name - NOTE: I'm keeping this around because copilot generated it and its kinda cool lol
    //     SDL_Color color = {255, 255, 255, 255};

    //     // get the current engine font size
    //     int og_size = TTF_FontHeight(YE_STATE.engine.pEngineFont);

    //     // set the size to something way less for performance reasons
    //     TTF_SetFontSize(YE_STATE.engine.pEngineFont, 32);

    //     SDL_Texture *text_texture = createTextTexture(current->entity->name, YE_STATE.engine.pEngineFont, &color);
    //     SDL_Rect text_rect = {entity_rect.x, entity_rect.y - 20, 0, 0};
    //     SDL_QueryTexture(text_texture, NULL, NULL, &text_rect.w, &text_rect.h);
    //     SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    //     SDL_DestroyTexture(text_texture); // TODO: cache for reusability somewhere and invalidate when name changes?

    //     // set the font size back to the original size
    //     TTF_SetFontSize(YE_STATE.engine.pEngineFont, og_size);
    // }

    // if(YE_STATE.editor.colliders_visible && current->entity->collider != NULL){
    //     // paint the collider
    //     SDL_Rect collider_rect = ye_convert_rectf_rect(ye_get_position(current->entity,YE_COMPONENT_COLLIDER));
    //     collider_rect.x = collider_rect.x - camera_rect.x;
    //     collider_rect.y = collider_rect.y - camera_rect.y;
    //     // yellow trigger collider
    //     if(current->entity->collider->is_trigger){
    //         SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    //         SDL_RenderDrawRect(renderer, &collider_rect);
    //         SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //     }
    //     // blue static collider
    //     else{
    //         SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    //         SDL_RenderDrawRect(renderer, &collider_rect);
    //         SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //     }
    // }
    // TODO: ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
}

/*
    Renderer v2, based on RenderGeometry

    TODO:
    - would be nice to work straight from cache in component since other parts of the engine need the vertex info we compute here
*/
void ye_renderer_v2(SDL_Renderer *renderer) {

    // reset stats
    YE_STATE.runtime.render_v2.num_render_calls = 0;
    YE_STATE.runtime.render_v2.num_verticies = 0;

    struct ye_entity *current_cam = YE_STATE.engine.target_camera;
    
    // check if we have a non-null, active camera targeted
    if (current_cam == NULL || current_cam->camera == NULL || !current_cam->camera->active) {
        ye_logf(warning, "No active camera targeted. Skipping renderer system\n");
        return;
    }

    if(YE_STATE.editor.editor_mode && YE_STATE.editor.editor_display_viewport_lines){
        _paint_viewport_lines(renderer);
    }

    YE_STATE.runtime.painted_entity_count = 0;

    /*
        Calculate verticies bounding camera

        TODO: does this account for rotations in the camera?
    */
    struct ye_point_rectf cam_prect = ye_rect_to_point_rectf(current_cam->camera->view_field);
    mat3_t cam_matrix = ye_get_offset_matrix(current_cam, YE_COMPONENT_CAMERA);
    // transform each vertex to get the final camera rect
    for(int i = 0; i < 4; i++){
        vec2_t point = {.data = {cam_prect.verticies[i].x, cam_prect.verticies[i].y}};
        point = lla_mat3_mult_vec2(cam_matrix, point);
        cam_prect.verticies[i].x = point.data[0];
        cam_prect.verticies[i].y = point.data[1];
    }
    
    // Traverse tracked entities with renderer components
    struct ye_entity_node *current = renderer_list_head;
    while (current != NULL) {
        if(!current->entity->renderer->active){
            current = current->next;
            continue;
        }

        // if this render object is an anim, tick it
        if(current->entity->renderer->type == YE_RENDERER_TYPE_ANIMATION)
            _attempt_tick_animation(current);
        
        // discard inactive/edge case entities
        if(!current->entity->active ||
            current->entity->renderer == NULL ||
            !current->entity->renderer->active ||
            current->entity->renderer->z > current_cam->camera->z
        ) {
            current = current->next;
            continue;
        }

        struct ye_component_renderer *rend = current->entity->renderer;
        struct ye_component_transform *trans = current->entity->transform;

        /*
            First, fit the AABB so we have a starting point to vertex-ify

            Take the local-space (both are translated to the origin) AABB's and
            figure out how to scale+translate child_AABB to fit inside bound_AABB
            fulfilling our stipulations.
        */
        struct ye_rectf bound_AABB = (struct ye_rectf){0, 0, rend->rect.w, rend->rect.h};
        struct ye_rectf child_AABB = ye_convert_rect_rectf(ye_get_real_texture_size_rect(rend->texture));
        
        /*
            If we are an animation or tmap tile, child_AABB is NOT the texture size!!
        */
        if(rend->type == YE_RENDERER_TYPE_ANIMATION){
            child_AABB = (struct ye_rectf){0, 0, rend->renderer_impl.animation->frame_width, rend->renderer_impl.animation->frame_height};
        }
        if(rend->type == YE_RENDERER_TYPE_TILEMAP_TILE){
            child_AABB = (struct ye_rectf){0, 0, rend->renderer_impl.tile->src.w, rend->renderer_impl.tile->src.h};
        }
        
        mat3_t align_mat = _get_auto_bound(&bound_AABB, &child_AABB, rend->alignment, !rend->preserve_original_size);

        /*
            Retrieve a rect comprised of floating point verticies in world space
        */
        // struct ye_rectf loc_rect = rend->rect;
        struct ye_rectf loc_rect = child_AABB;
        loc_rect.x = 0;
        loc_rect.y = 0;
        struct ye_point_rectf entity_prect = ye_rect_to_point_rectf(loc_rect);

        /*
            Shift matrix to transfer from "local" to "world" space
        */
        mat3_t world_matrix = lla_mat3_identity();
        if(rend->relative) {
            world_matrix = lla_mat3_translate(world_matrix, (vec2_t){.data = {trans->x, trans->y}}); // apply transform if it's relative to it
        }
        world_matrix = lla_mat3_translate(world_matrix, (vec2_t){.data = {rend->rect.x, rend->rect.y}}); // always offset renderer pos

        /*
            Create rotation matrix, encompassing transform and renderer rotation.

            These are performed by scaling to the origin (local) and rotating, then back.
        */
        mat3_t rotation_mat = lla_mat3_identity();
        if(trans && trans->rotation != 0){
            rotation_mat = lla_mat3_translate(rotation_mat, (vec2_t){.data = {trans->x, trans->y}});
            rotation_mat = lla_mat3_rotate(rotation_mat, trans->rotation);
            rotation_mat = lla_mat3_translate(rotation_mat, (vec2_t){.data = {-trans->x, -trans->y}});
        }
        if(rend->rotation != 0) {
            vec2_t pivot;
            if(rend->relative) {
                // Relative rotation - pivot around center point relative to transform + renderer offset
                pivot = (vec2_t){.data = {
                    trans->x + rend->rect.x + rend->center.x,
                    trans->y + rend->rect.y + rend->center.y
                }};
            } else {
                // Absolute rotation - pivot around center point in world space
                pivot = (vec2_t){.data = {
                    rend->center.x,
                    rend->center.y
                }};
            }
        
            // Translate to pivot point
            rotation_mat = lla_mat3_translate(rotation_mat, pivot);
            // Rotate around pivot
            rotation_mat = lla_mat3_rotate(rotation_mat, rend->rotation);
            // Translate back from pivot
            rotation_mat = lla_mat3_translate(rotation_mat, 
                (vec2_t){.data = {-pivot.data[0], -pivot.data[1]}});
        }

        /*
            We can now immediately apply the alignment and rotation matricies to our world space verticies.

            Initialize the verticies now.
        */
        SDL_Vertex * world_verts = rend->_world_verts;
        SDL_Vertex * cam_verts = rend->_cam_verts;
        int * indicies = rend->_indicies;

        // TODO: optimize this to a constructor somehere, im leaving for clarity
        indicies[0] = 0;
        indicies[1] = 1;
        indicies[2] = 2;
        indicies[3] = 2;
        indicies[4] = 3;
        indicies[5] = 0;

        for(int i = 0; i < 4; i++){
            vec2_t v = {.data = {entity_prect.verticies[i].x, entity_prect.verticies[i].y}};
            v = lla_mat3_mult_vec2(align_mat, v);
            v = lla_mat3_mult_vec2(rotation_mat, v);
            v = lla_mat3_mult_vec2(world_matrix, v);

            world_verts[i].position.x = v.data[0];
            world_verts[i].position.y = v.data[1];
        }

        // matrix which transforms back to "window" coordinates
        // TODO: this might be why we need to offset camera location in util.c
        mat3_t world2cam = lla_mat3_inverse(cam_matrix);

        /*
        
            TODO: renderer2

            - refactor the component paintbounds
        */

        /*
            For rendering, afaict RenderGeometry only takes triangles,
            so we need to port into SDL_Vertex and a list of indicies

            1---2
            |   |
            0---3
        */

        // compute ahead so we can use for collision detection
        struct ye_point_rectf local_rect;

        // Translate all verticies from world to camera
        for(int i = 0; i < 4; i++){
            // transform from world into camera space
            vec2_t point = {.data = {world_verts[i].position.x, world_verts[i].position.y}};
            point = lla_mat3_mult_vec2(world2cam, point);
            cam_verts[i].position.x = point.data[0];
            cam_verts[i].position.y = point.data[1];

            // set color
            cam_verts[i].color.r = 255;
            cam_verts[i].color.g = 255;
            cam_verts[i].color.b = 255;
            cam_verts[i].color.a = current->entity->renderer->alpha;

            // cache
            local_rect.verticies[i].x = point.data[0];
            local_rect.verticies[i].y = point.data[1];
        }

        /*
            TODO: before we set UV's or render, we need to
            check if at least one edge is intersecting the camera
        */

        // compute cam_prect in local space
        struct ye_point_rectf local_cam_prect = cam_prect;
        for(int i = 0; i < 4; i++){
            vec2_t point = {.data = {cam_prect.verticies[i].x, cam_prect.verticies[i].y}};
            point = lla_mat3_mult_vec2(world2cam, point);
            local_cam_prect.verticies[i].x = point.data[0];
            local_cam_prect.verticies[i].y = point.data[1];
        }

        if(!ye_detect_rect_rect_collision(local_rect, local_cam_prect)) {
            current = current->next;
            continue;
        }

        /*
            If we are painting wireframes, skip all the overhead
        */
        if(YE_STATE.editor.wireframe_visible && current->entity->name != NULL && strcmp(current->entity->name, "origin") != 0) {

            /*
                To save cycles, we will just paint the quad outline, and then add the diagonal
            */
            for(int i = 0; i < 4; i++){
                float x1 = cam_verts[i].position.x;
                float y1 = cam_verts[i].position.y;
                float x2 = cam_verts[(i + 1) % 4].position.x;
                float y2 = cam_verts[(i + 1) % 4].position.y;

                ye_draw_thick_line(renderer, x1, y1, x2, y2, 4, (SDL_Color){255, 0, 0, 255});
            }
            ye_draw_thick_line(renderer, cam_verts[0].position.x, cam_verts[0].position.y, cam_verts[2].position.x, cam_verts[2].position.y, 4, (SDL_Color){255, 0, 0, 255});

            YE_STATE.runtime.render_v2.num_render_calls++;
            YE_STATE.runtime.render_v2.num_verticies += 4; // TODO: dont think I can do sizeof because of pointer decay
            YE_STATE.runtime.painted_entity_count++;
            current = current->next;
            continue;
        }

        /*
            By default, our uvs span the whole texture,
            but for animations and tilemaps we must compute
            the normalized uv float
        */
        float tcx_start = 0;
        float tcx_end   = 1;
        float tcy_start = 0;
        float tcy_end   = 1;

        /*
            Animations are comprised of vertical atlas, meaning w=frame_width h=frame_height*num_frames
        */
        if(current->entity->renderer->type == YE_RENDERER_TYPE_ANIMATION){
            struct ye_component_renderer * rend = current->entity->renderer;
            tcy_start = (float)rend->renderer_impl.animation->current_frame_index / (float)rend->renderer_impl.animation->frame_count;
            tcy_end = (float)(rend->renderer_impl.animation->current_frame_index + 1) / (float)rend->renderer_impl.animation->frame_count;
        }

        /*
            We lack the meta in struct ye_component_renderer to determine the size and offset of the tilemap in the image,
            so this is a performance hit workaround.

            TODO: cache this or wrap texture in a meta-preserving struct
        */
        if(current->entity->renderer->type == YE_RENDERER_TYPE_TILEMAP_TILE){
            int w, h;
            SDL_QueryTexture(current->entity->renderer->texture, NULL, NULL, &w, &h);
        
            SDL_Rect *src = &current->entity->renderer->renderer_impl.tile->src;
            
            tcx_start = (float)src->x / (float)w;
            tcx_end = (float)(src->x + src->w) / (float)w;
            tcy_start = (float)src->y / (float)h;
            tcy_end = (float)(src->y + src->h) / (float)h;
        }

        // set texcoord (shoutout gpt4 for the flipped_n computation)
        bool flipped_x = current->entity->renderer->flipped_x;
        bool flipped_y = current->entity->renderer->flipped_y;
        float tex_coords[4][2] = {
            {flipped_x ? tcx_end : tcx_start, flipped_y ? tcy_end : tcy_start},
            {flipped_x ? tcx_end : tcx_start, flipped_y ? tcy_start : tcy_end},
            {flipped_x ? tcx_start : tcx_end, flipped_y ? tcy_start : tcy_end},
            {flipped_x ? tcx_start : tcx_end, flipped_y ? tcy_end : tcy_start}
        };
        for (int i = 0; i < 4; i++) {
            cam_verts[i].tex_coord.x = tex_coords[i][0];
            cam_verts[i].tex_coord.y = tex_coords[i][1];
        }

        SDL_RenderGeometry(renderer, current->entity->renderer->texture, cam_verts, 4, indicies, 6);

        YE_STATE.runtime.render_v2.num_render_calls++;
        YE_STATE.runtime.render_v2.num_verticies += 4; // TODO: dont think I can do sizeof because of pointer decay

        YE_STATE.runtime.painted_entity_count++;
        
        // TODO: prect refactor
        _paint_paintbounds(renderer, current);

        current = current->next;
    }

    /*
        additional post processing for editor mode    
        RUNS ONCE AFTER ALL ENTITES ARE PAINTED
    */
    // if(YE_STATE.editor.editor_mode && YE_STATE.editor.scene_camera_bounds_visible && YE_STATE.editor.scene_default_camera != NULL){
    //     // draw box around viewport of engine_runtime_state.scene_default_camera
    //     SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    //     SDL_Rect scene_camera_rect = ye_get_position_rect(YE_STATE.editor.scene_default_camera,YE_COMPONENT_CAMERA);
    //     scene_camera_rect.x = scene_camera_rect.x - camera_rect.x;
    //     scene_camera_rect.y = scene_camera_rect.y - camera_rect.y;
    //     scene_camera_rect.w = scene_camera_rect.w;
    //     scene_camera_rect.h = scene_camera_rect.h;
    //     SDL_RenderDrawRect(renderer, &scene_camera_rect);
    //     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // }
    // TODO: ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    /*
        Additional render step to allow the game to perform custom behavior
        TODO: removeme?
        - 8/18/24, im just gonna leave for posterity
    */
    ye_fire_event(YE_EVENT_ADDITIONAL_RENDER, (union ye_event_args){NULL});

    /*
        Perform additional immediate and callback based rendering on top of the frame we have just prepared
    */
    ye_debug_renderer_render();
}