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

#include <string.h>

#include <jansson.h>

#include <yoyoengine/yep.h>
#include <yoyoengine/json.h>
#include <yoyoengine/cache.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/ecs/camera.h>
#include <yoyoengine/ecs/renderer.h>
#include <yoyoengine/ecs/collider.h>
#include <yoyoengine/debug_renderer.h>

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
        return;
    }

    // version of the file
    int version; ye_json_int(META, "version", &version);
    if(version != YE_ENGINE_ANIMATION_FILE_VERSION){
        ye_logf(error, "Invalid animation meta file version %d against %d\n", version, YE_ENGINE_ANIMATION_FILE_VERSION);
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

void ye_system_renderer(SDL_Renderer *renderer) {
    /*
        These lines really suck and I wish they were better,
        probably need actual zoom field tracking in the camera.
    */
    if(YE_STATE.editor.editor_mode && YE_STATE.editor.editor_display_viewport_lines){
        // draw a grid of white evently spaced lines across the screen
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50);
        for(int i = 0; i < YE_STATE.engine.target_camera->camera->view_field.w; i += 32){
            SDL_RenderDrawLine(renderer, i, 0, i, YE_STATE.engine.target_camera->camera->view_field.h);
        }
        for(int i = 0; i < YE_STATE.engine.target_camera->camera->view_field.h; i += 32){
            SDL_RenderDrawLine(renderer, 0, i, YE_STATE.engine.target_camera->camera->view_field.w, i);
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        SDL_Rect cam = ye_get_position_rect(YE_STATE.engine.target_camera,YE_COMPONENT_CAMERA);

        // x axis
        ye_draw_thick_line(
            renderer,
            0,
            0 - cam.y,
            cam.w,
            0 - cam.y,
            6,
            (SDL_Color){255, 255, 255, 255}
        );

        // y axis
        ye_draw_thick_line(
            renderer,
            0 - cam.x,
            0,
            0 - cam.x,
            cam.h,
            6,
            (SDL_Color){255, 255, 255, 255}
        );

        /*
            TODO: experimental subsectioning lines
        */
        // int num_lines = 5;
        // int line_spacing = cam.w / num_lines;
        // int x_offset = cam.x % line_spacing;

        // for(int i = 1; i < num_lines; i++){
        //     ye_draw_thick_line(
        //         renderer,
        //         (i * line_spacing) - x_offset,
        //         0,
        //         (i * line_spacing) - x_offset,
        //         cam.h,
        //         2,
        //         (SDL_Color){255, 0, 0, 255}
        //     );
        // }
        
        // int y_offset = cam.y % line_spacing;

        // for(int i = 1; i < num_lines; i++){
        //     ye_draw_thick_line(
        //         renderer,
        //         0,
        //         (i * line_spacing) - y_offset,
        //         cam.w,
        //         (i * line_spacing) - y_offset,
        //         2,
        //         (SDL_Color){0, 255, 0, 255}
        //     );
        // }
    }

    // check if we have a non-null, active camera targeted
    if (YE_STATE.engine.target_camera == NULL || YE_STATE.engine.target_camera->camera == NULL || !YE_STATE.engine.target_camera->camera->active) {
        ye_logf(warning, "No active camera targeted. Skipping renderer system\n");
        return;
    }

    YE_STATE.runtime.painted_entity_count = 0;

    /*
        Get the cameras position in world coordinates
    */
    SDL_Rect camera_rect = ye_get_position_rect(YE_STATE.engine.target_camera,YE_COMPONENT_CAMERA);

    // Traverse tracked entities with renderer components
    struct ye_entity_node *current = renderer_list_head;
    while (current != NULL) {
        if (current->entity->renderer->active) {
            // check if renderer is animation and attemt to tick its frame if so
            // TODO: this should be decoupled from the renderer and become its own system
            if(current->entity->renderer->type == YE_RENDERER_TYPE_ANIMATION){
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
                            if(animation->current_frame_index >= animation->frame_count){
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
            // paint the entity
            if (current->entity->active && // entity active
                current->entity->renderer != NULL && // renderer is not null
                current->entity->renderer->active && // renderer is active
                current->entity->renderer->z <= YE_STATE.engine.target_camera->camera->z // only render if the entity is on or in front of the camera
                // current->entity->transform != NULL &&
                // current->entity->transform->active &&
                // ^ old system that relied on transform
            ) {
                struct ye_rectf temp_entity_rect = ye_get_position(current->entity,YE_COMPONENT_RENDERER);
                struct ye_rectf texture_rect;
                if(current->entity->renderer->type != YE_RENDERER_TYPE_ANIMATION)
                    texture_rect = ye_convert_rect_rectf(ye_get_real_texture_size_rect(current->entity->renderer->texture));
                else
                    texture_rect = (struct ye_rectf){0, 0, current->entity->renderer->renderer_impl.animation->frame_width, current->entity->renderer->renderer_impl.animation->frame_height};
                
                ye_auto_fit_bounds(&temp_entity_rect, &texture_rect, current->entity->renderer->alignment, &current->entity->renderer->center, !current->entity->renderer->preserve_original_size);
                SDL_Rect entity_rect = ye_convert_rectf_rect(texture_rect);

                // update computed bounds field //
                current->entity->renderer->computed_pos = texture_rect;
                //////////////////////////////////

                // entity rect is now a reflection of the actual calculated rect

                /*
                    TODO: HACK FOR ACEROLA JAM ZERO, THERE IS A BETTER WAY TO FORMULAICALLY DETERMINE THIS OFFSET TO CUT
                    DOWN ON RENDERING COSTS
                    
                    still render things NEAR the camera if they are rotated so we dont get artifacts for rotated sprites
                    (popping out while still in view, not accounting for rotation)
                */
                int occlusion_offset_x = 0;
                int occlusion_offset_y = 0;
                if(current->entity->renderer->rotation != 0){
                    occlusion_offset_x = YE_STATE.engine.target_camera->camera->view_field.w;
                    occlusion_offset_y = YE_STATE.engine.target_camera->camera->view_field.h;
                    // occlusion_offset_x = 1920;
                    // occlusion_offset_y = 1080;
                }

                // occlusion check
                if (entity_rect.x + entity_rect.w < camera_rect.x - occlusion_offset_x ||
                    entity_rect.x > camera_rect.x + camera_rect.w + occlusion_offset_x ||
                    entity_rect.y + entity_rect.h < camera_rect.y - occlusion_offset_y ||
                    entity_rect.y > camera_rect.y + camera_rect.h + occlusion_offset_y
                    )
                {
                    // do not draw the object
                    // log that we occluded entity and its name
                    // ye_logf(debug, "Occluded entity %s\n", current->entity->name);
                }
                else{
                    // set alpha (log failure) TODO: profile efficiency of this
                    if (SDL_SetTextureAlphaMod(current->entity->renderer->texture, current->entity->renderer->alpha) != 0) {
                        ye_logf(warning, "Failed to set alpha for entity %s\n", current->entity->name);
                        // log the sdl get error
                        ye_logf(warning, "SDL_GetError: %s\n", SDL_GetError());
                    }

                    // scale it to be on screen and paint it
                    entity_rect.x = entity_rect.x - camera_rect.x;
                    entity_rect.y = entity_rect.y - camera_rect.y;

                    /*
                        If the renderer is a tilemap tile, we need to set the src rect to the tilemap tile src rect
                        Else, just render the full source image

                        NOTE: in the future, this will probably also point to the correct frame of an animation
                    */
                    SDL_Rect *ent_src_rect = NULL;
                    if(current->entity->renderer->type == YE_RENDERER_TYPE_TILEMAP_TILE){
                        ent_src_rect = &current->entity->renderer->renderer_impl.tile->src;
                    }

                    /*
                        For an animation, the src rect is
                        {
                            x: 0,
                            y: current_frame_index * frame_height,
                            w: frame_width,
                            h: frame_height
                        }
                    */
                    if(current->entity->renderer->type == YE_RENDERER_TYPE_ANIMATION){
                        struct ye_component_renderer * rend = current->entity->renderer;
                        ent_src_rect = &(SDL_Rect){
                            0,
                            rend->renderer_impl.animation->current_frame_index * rend->renderer_impl.animation->frame_height,
                            rend->renderer_impl.animation->frame_width,
                            rend->renderer_impl.animation->frame_height
                        };
                        // printf("ent_src_rect: %d %d %d %d\n", ent_src_rect->x, ent_src_rect->y, ent_src_rect->w, ent_src_rect->h);
                    }

                    if(current->entity->renderer->alignment == YE_ALIGN_STRETCH)
                        ent_src_rect = NULL;

                    // if transform is flipped or rotated render it differently
                    if(current->entity->renderer->flipped_x || current->entity->renderer->flipped_y){
                        SDL_RendererFlip flip = SDL_FLIP_NONE;
                        if(current->entity->renderer->flipped_x && current->entity->renderer->flipped_y){
                            flip = SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL;
                        }
                        else if(current->entity->renderer->flipped_x){
                            flip = SDL_FLIP_HORIZONTAL;
                        }
                        else if(current->entity->renderer->flipped_y){
                            flip = SDL_FLIP_VERTICAL;
                        }
                        SDL_RenderCopyEx(renderer, current->entity->renderer->texture, ent_src_rect, &entity_rect, (int)current->entity->renderer->rotation, NULL, flip);
                    }
                    else if(current->entity->renderer->rotation != 0.0){
                        SDL_RenderCopyEx(renderer, current->entity->renderer->texture, ent_src_rect, &entity_rect, (int)current->entity->renderer->rotation, &current->entity->renderer->center, SDL_FLIP_NONE);
                    }
                    else{
                        SDL_RenderCopy(renderer, current->entity->renderer->texture, ent_src_rect, &entity_rect);
                    }
                    
                    YE_STATE.runtime.painted_entity_count++;
                    
                    // paint bounds, my beloved <3
                    if (YE_STATE.editor.paintbounds_visible) {
                        // create entity bounds offset by camera
                        // SDL_Rect entity_bounds = ye_convert_rectf_rect(current->entity->transform->bounds);
                        // entity_bounds.x = entity_bounds.x - camera_rect.x;
                        // entity_bounds.y = entity_bounds.y - camera_rect.y;
                        
                        // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                        // SDL_RenderDrawRect(renderer, &entity_bounds);
                        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                        SDL_RenderDrawRect(renderer, &entity_rect);

                        // paint an orange rectangle filled at the entity center (transform->center) SDL_Point
                        SDL_Rect center_rect = {entity_rect.x + current->entity->renderer->center.x - 10, entity_rect.y + current->entity->renderer->center.y - 10, 20, 20};
                        SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
                        SDL_RenderFillRect(renderer, &center_rect);
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    }

                    // button bounds
                    if(current->entity->button != NULL && YE_STATE.editor.button_bounds_visible){
                        // paint the button bounds
                        SDL_Rect button_bounds = ye_convert_rectf_rect(ye_get_position(current->entity,YE_COMPONENT_BUTTON));
                        // printf("button_bounds: %d %d %d %d\n", button_bounds.x, button_bounds.y, button_bounds.w, button_bounds.h);
                        button_bounds.x = button_bounds.x - camera_rect.x;
                        button_bounds.y = button_bounds.y - camera_rect.y;
                        SDL_SetRenderDrawColor(renderer, 235, 52, 235, 255);
                        SDL_RenderDrawRect(renderer, &button_bounds);
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    }

                    // audio range
                    if(current->entity->audiosource != NULL && YE_STATE.editor.audiorange_visible){
                        SDL_Rect audio_range_rect = ye_convert_rectf_rect(
                            ye_get_position(current->entity,YE_COMPONENT_AUDIOSOURCE)
                        );
                        audio_range_rect.x = audio_range_rect.x - camera_rect.x;
                        audio_range_rect.y = audio_range_rect.y - camera_rect.y;
                        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

                        // use ye_draw_circle to draw the audio range, the width is the radius
                        ye_draw_circle(renderer, audio_range_rect.x + (audio_range_rect.w / 2), audio_range_rect.y + (audio_range_rect.h / 2), audio_range_rect.w / 2);

                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    }

                    if(YE_STATE.editor.editor_mode && YE_STATE.editor.display_names){
                        // paint the entity name - NOTE: I'm keeping this around because copilot generated it and its kinda cool lol
                        SDL_Color color = {255, 255, 255, 255};
                        SDL_Texture *text_texture = createTextTexture(current->entity->name, YE_STATE.engine.pEngineFont, &color);
                        SDL_Rect text_rect = {entity_rect.x, entity_rect.y - 20, 0, 0};
                        SDL_QueryTexture(text_texture, NULL, NULL, &text_rect.w, &text_rect.h);
                        SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
                        SDL_DestroyTexture(text_texture);
                    }

                    if(YE_STATE.editor.colliders_visible && current->entity->collider != NULL){
                        // paint the collider
                        SDL_Rect collider_rect = ye_convert_rectf_rect(ye_get_position(current->entity,YE_COMPONENT_COLLIDER));
                        collider_rect.x = collider_rect.x - camera_rect.x;
                        collider_rect.y = collider_rect.y - camera_rect.y;
                        // yellow trigger collider
                        if(current->entity->collider->is_trigger){
                            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                            SDL_RenderDrawRect(renderer, &collider_rect);
                            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        }
                        // blue static collider
                        else{
                            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                            SDL_RenderDrawRect(renderer, &collider_rect);
                            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        }
                    }
                }
            }
        }
        current = current->next;
    }

    /*
        additional post processing for editor mode    
        RUNS ONCE AFTER ALL ENTITES ARE PAINTED
    */
    if(YE_STATE.editor.editor_mode && YE_STATE.editor.scene_camera_bounds_visible && YE_STATE.editor.scene_default_camera != NULL){
        // draw box around viewport of engine_runtime_state.scene_default_camera
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect scene_camera_rect = ye_get_position_rect(YE_STATE.editor.scene_default_camera,YE_COMPONENT_CAMERA);
        scene_camera_rect.x = scene_camera_rect.x - camera_rect.x;
        scene_camera_rect.y = scene_camera_rect.y - camera_rect.y;
        scene_camera_rect.w = scene_camera_rect.w;
        scene_camera_rect.h = scene_camera_rect.h;
        SDL_RenderDrawRect(renderer, &scene_camera_rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }

    // TODO: removeme?
    // if(YE_STATE.editor.editor_mode && YE_STATE.editor.selected_entity != NULL && YE_STATE.editor.selected_entity->renderer != NULL){
    //     // draw a pink rect around the selected entity rect
    //     SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
    //     SDL_Rect selected_entity_rect = ye_get_position_rect(YE_STATE.editor.selected_entity,YE_COMPONENT_RENDERER);
    //     selected_entity_rect.x = selected_entity_rect.x - camera_rect.x;
    //     selected_entity_rect.y = selected_entity_rect.y - camera_rect.y;
    //     SDL_RenderDrawRect(renderer, &selected_entity_rect);
    //     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // }

    /*
        Additional render step to allow the game to perform custom behavior
        TODO: removeme?
    */
    if(YE_STATE.engine.callbacks.additional_render != NULL)
        YE_STATE.engine.callbacks.additional_render();

    /*
        Perform additional immediate and callback based rendering on top of the frame we have just prepared
    */
    ye_debug_renderer_render();
}