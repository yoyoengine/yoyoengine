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

void ye_add_renderer_component(struct ye_entity *entity, enum ye_component_renderer_type type, void *data){
    entity->renderer = malloc(sizeof(struct ye_component_renderer));
    entity->renderer->active = true;
    entity->renderer->type = type;
    entity->renderer->alpha = 255; // by default renderer is fully opaque
    entity->renderer->flipped_x = false;
    entity->renderer->flipped_y = false;

    if(type == YE_RENDERER_TYPE_IMAGE){
        entity->renderer->renderer_impl.image = data;
    }
    else if(type == YE_RENDERER_TYPE_TEXT){
        entity->renderer->renderer_impl.text = data;
    }
    else if(type == YE_RENDERER_TYPE_ANIMATION){
        entity->renderer->renderer_impl.animation = data;
    }

    // add this entity to the renderer component list
    ye_entity_list_add_sorted_z(&renderer_list_head, entity);

    // log that we added a renderer and to what ID
    // ye_logf(debug, "Added renderer to entity %d\n", entity->id);
}

void ye_temp_add_image_renderer_component(struct ye_entity *entity, char *src){
    struct ye_component_renderer_image *image = malloc(sizeof(struct ye_component_renderer_image));
    // copy src to image->src
    image->src = malloc(sizeof(char) * strlen(src));

    // create the renderer top level
    ye_add_renderer_component(entity, YE_RENDERER_TYPE_IMAGE, image);

    // create the image texture
    entity->renderer->texture = ye_image(src);

    
    if(entity->transform != NULL){
        // calculate the actual rect of the entity based on its alignment and bounds
        entity->transform->rect = ye_convert_rect_rectf(
            ye_get_real_texture_size_rect(entity->renderer->texture)
        );
        ye_auto_fit_bounds(&entity->transform->bounds, &entity->transform->rect, entity->transform->alignment, &entity->transform->center);
    }
    else{
        ye_logf(warning, "Entity has renderer but no transform. Its real paint bounds have not been computed\n");
    }
}

void ye_temp_add_text_renderer_component(struct ye_entity *entity, char *text, TTF_Font *font, SDL_Color *color){
    struct ye_component_renderer_text *text_renderer = malloc(sizeof(struct ye_component_renderer_text));
    text_renderer->text = strdup(text);
    text_renderer->font = font;
    text_renderer->color = color;

    // create the renderer top level
    ye_add_renderer_component(entity, YE_RENDERER_TYPE_TEXT, text_renderer);

    // create the text texture
    entity->renderer->texture = createTextTexture(text, font, color);

    if(entity->transform != NULL){
        // calculate the actual rect of the entity based on its alignment and bounds
        entity->transform->rect = ye_convert_rect_rectf(
            ye_get_real_texture_size_rect(entity->renderer->texture)
        );
        ye_auto_fit_bounds(&entity->transform->bounds, &entity->transform->rect, entity->transform->alignment, &entity->transform->center);
    }
    else{
        ye_logf(warning, "Entity has renderer but no transform. Its real paint bounds have not been computed\n");
    }
}

void ye_temp_add_text_outlined_renderer_component(struct ye_entity *entity, char *text, TTF_Font *font, SDL_Color *color, SDL_Color *outline_color, int outline_size){
    struct ye_component_renderer_text_outlined *text_renderer = malloc(sizeof(struct ye_component_renderer_text_outlined));
    text_renderer->text = strdup(text);
    text_renderer->font = font;
    text_renderer->color = color;
    text_renderer->outline_color = outline_color;
    text_renderer->outline_size = outline_size;

    // create the renderer top level
    ye_add_renderer_component(entity, YE_RENDERER_TYPE_TEXT_OUTLINED, text_renderer);

    // create the text texture
    entity->renderer->texture = createTextTextureWithOutline(text,outline_size,font,color,outline_color);

    if(entity->transform != NULL){
        // calculate the actual rect of the entity based on its alignment and bounds
        entity->transform->rect = ye_convert_rect_rectf(
            ye_get_real_texture_size_rect(entity->renderer->texture)
        );
        ye_auto_fit_bounds(&entity->transform->bounds, &entity->transform->rect, entity->transform->alignment, &entity->transform->center);
    }
    else{
        ye_logf(warning, "Entity has renderer but no transform. Its real paint bounds have not been computed\n");
    }
}

void ye_temp_add_animation_renderer_component(struct ye_entity *entity, char *path, char *format, size_t count, int frame_delay, int loops){
    struct ye_component_renderer_animation *animation = malloc(sizeof(struct ye_component_renderer_animation));
    animation->animation_path = strdup(path);
    animation->image_format = strdup(format);
    animation->frame_count = count;
    animation->frame_delay = frame_delay;
    animation->loops = loops;
    animation->last_updated = 0; // set as 0 now so the operations between now and setting it do not count towards its frame time
    animation->current_frame_index = 0;
    animation->paused = false;
    animation->frames = (SDL_Texture**)malloc(count * sizeof(SDL_Texture*));
 
    // load all the frames into memory TODO: this could be futurely optimized
    for (size_t i = 0; i < (size_t)count; ++i) {
        char filename[256];  // Assuming a maximum filename length of 255 characters
        snprintf(filename, sizeof(filename), "%s/%d.%s", path, (int)i, format); // TODO: dumb optimization but could cut out all except frame num insertion here
        animation->frames[i] = ye_image(filename);
    }

    // create the renderer top level
    ye_add_renderer_component(entity, YE_RENDERER_TYPE_ANIMATION, animation);

    // set the texture to the first frame
    entity->renderer->texture = animation->frames[0];

    // make sure we are aligned
    if(entity->transform != NULL){
        // calculate the actual rect of the entity based on its alignment and bounds
        entity->transform->rect = ye_convert_rect_rectf(
            ye_get_real_texture_size_rect(entity->renderer->texture)
        );
        ye_auto_fit_bounds(&entity->transform->bounds, &entity->transform->rect, entity->transform->alignment, &entity->transform->center);
    }
    else{
        ye_logf(warning, "Entity has renderer but no transform. Its real paint bounds have not been computed\n");
    }


    animation->last_updated = SDL_GetTicks(); // set the last updated to now so we can start ticking it accurately
}

void ye_remove_renderer_component(struct ye_entity *entity){
    // free contents of renderer_impl
    if(entity->renderer->type == YE_RENDERER_TYPE_IMAGE){
        free(entity->renderer->renderer_impl.image->src);
        free(entity->renderer->renderer_impl.image);
    }
    else if(entity->renderer->type == YE_RENDERER_TYPE_TEXT){
        free(entity->renderer->renderer_impl.text->text);
        free(entity->renderer->renderer_impl.text);
    }
    else if(entity->renderer->type == YE_RENDERER_TYPE_ANIMATION){
        // cache will handle freeing the frames as needed

        free(entity->renderer->renderer_impl.animation->animation_path);
        free(entity->renderer->renderer_impl.animation->image_format);
        free(entity->renderer->renderer_impl.animation->frames);
        free(entity->renderer->renderer_impl.animation);
    }

    // cache will handle freeing the texture as needed

    free(entity->renderer);
    entity->renderer = NULL;

    // remove the entity from the renderer component list
    ye_entity_list_remove(&renderer_list_head, entity);
}

/*
    Renderer system

    Acts upon the list of tracked entities with renderers and paints them
    to the screen.

    Uses the transform component to determine where to paint the entity.
    Skips entity if there is no active transform or renderer is inactive

    This system will paint relative to the active camera, and occlude anything
    outside of the active camera's view field

    TODO:
    - fulcrum cull rotated entities

    NOTES:
    - Initially I tried some weird really complicated impl, which I will share details
    of below, for future me who will likely need these in the future.
    For now, we literally just check an intersection of the object and the camera, and if it exists we
    copy the object to the renderer offset by the camera position.
    In the future we might want to return to the weird clip plane system, but for now this is fine.
    - I'm making a REALLY stupid assumption that we dont really scale the camera viewfield outside of the
    window resolution. I dont want to deal with the paint issues that i need to solve to truly paint from the cameras POV

    Some functions and relevant snippets to the old system:
    - SDL_RenderSetClipRect(renderer, &visibleRect);
    - SDL_Rect rect = {0,0,640,320}; SDL_RenderSetViewport(pRenderer, &rect);
*/
void ye_system_renderer(SDL_Renderer *renderer) {
    // if we are in editor mode
    if(engine_state.editor_mode && engine_runtime_state.editor_display_viewport_lines){
        // draw a grid of white evently spaced lines across the screen
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50);
        for(int i = 0; i < engine_state.target_camera->camera->view_field.w; i += 32){
            SDL_RenderDrawLine(renderer, i, 0, i, engine_state.target_camera->camera->view_field.h);
        }
        for(int i = 0; i < engine_state.target_camera->camera->view_field.h; i += 32){
            SDL_RenderDrawLine(renderer, 0, i, engine_state.target_camera->camera->view_field.w, i);
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    // check if we have a non-null, active camera targeted
    if (engine_state.target_camera == NULL || engine_state.target_camera->camera == NULL || !engine_state.target_camera->camera->active) {
        ye_logf(warning, "No active camera targeted. Skipping renderer system\n");
        return;
    }

    engine_runtime_state.painted_entity_count = 0;

    // Get the camera's position in world coordinates
    SDL_Rect camera_rect = ye_convert_rectf_rect(engine_state.target_camera->transform->rect); // TODO profile conversion overhead
    
    SDL_Rect view_field = engine_state.target_camera->camera->view_field;
    camera_rect.w = view_field.w;
    camera_rect.h = view_field.h;
    // update camera rect to contain the view field w,h

    // Traverse tracked entities with renderer components
    struct ye_entity_node *current = renderer_list_head;
    while (current != NULL) {
        if (current->entity->renderer->active) {
            // check if renderer is animation and attemt to tick its frame if so
            // TODO: this should be decoupled from the renderer and become its own system
            if(current->entity->renderer->type == YE_RENDERER_TYPE_ANIMATION){
                // if not editor mode (we want to not run animations in editor)
                if(!engine_state.editor_mode){
                    struct ye_component_renderer_animation *animation = current->entity->renderer->renderer_impl.animation;
                    if(!animation->paused){
                        int now = SDL_GetTicks();
                        if(now - animation->last_updated >= animation->frame_delay){
                            animation->current_frame_index++;
                            if(animation->current_frame_index >= animation->frame_count){
                                animation->current_frame_index = 0;
                                if(animation->loops != -1){
                                    animation->loops--;
                                    if(animation->loops == 0){
                                        animation->paused = true; // TODO: dont just pause when it ends, but give option to destroy/ disable renderer
                                    }
                                }
                            }
                            animation->last_updated = now;
                            current->entity->renderer->texture = animation->frames[animation->current_frame_index];
                        }
                    }
                }
            }
            // paint the entity
            if (current->entity->active &&
                current->entity->transform != NULL && current->entity->renderer != NULL && current->entity->renderer->active &&
                current->entity->transform->active &&
                current->entity->transform->z <= engine_state.target_camera->transform->z // only render if the entity is on or in front of the camera
            ) {
                SDL_Rect entity_rect = ye_convert_rectf_rect(current->entity->transform->rect); // where the entity is in the world by pixel (x, y, w, h)

                // occlusion check
                if (entity_rect.x + entity_rect.w < camera_rect.x ||
                    entity_rect.x > camera_rect.x + camera_rect.w ||
                    entity_rect.y + entity_rect.h < camera_rect.y ||
                    entity_rect.y > camera_rect.y + camera_rect.h
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
                        SDL_RenderCopyEx(renderer, current->entity->renderer->texture, NULL, &entity_rect, (int)current->entity->transform->rotation, NULL, flip);
                    }
                    else if(current->entity->transform->rotation != 0.0){
                        SDL_RenderCopyEx(renderer, current->entity->renderer->texture, NULL, &entity_rect, (int)current->entity->transform->rotation, &current->entity->transform->center, SDL_FLIP_NONE);
                    }
                    else{
                        SDL_RenderCopy(renderer, current->entity->renderer->texture, NULL, &entity_rect);
                    }
                    
                    engine_runtime_state.painted_entity_count++;
                    
                    // paint bounds, my beloved <3
                    if (engine_state.paintbounds_visible) {
                        // create entity bounds offset by camera
                        SDL_Rect entity_bounds = ye_convert_rectf_rect(current->entity->transform->bounds);
                        entity_bounds.x = entity_bounds.x - camera_rect.x;
                        entity_bounds.y = entity_bounds.y - camera_rect.y;
                        
                        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                        SDL_RenderDrawRect(renderer, &entity_bounds);
                        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                        SDL_RenderDrawRect(renderer, &entity_rect);

                        // paint an orange rectangle filled at the entity center (transform->center) SDL_Point
                        SDL_Rect center_rect = {entity_rect.x + current->entity->transform->center.x - 10, entity_rect.y + current->entity->transform->center.y - 10, 20, 20};
                        SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
                        SDL_RenderFillRect(renderer, &center_rect);
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    }

                    if(engine_state.editor_mode && engine_runtime_state.display_names){
                        // paint the entity name - NOTE: I'm keeping this around because copilot generated it and its kinda cool lol
                        SDL_Color color = {255, 255, 255, 255};
                        SDL_Texture *text_texture = createTextTexture(current->entity->name, pEngineFont, &color);
                        SDL_Rect text_rect = {entity_rect.x, entity_rect.y - 20, 0, 0};
                        SDL_QueryTexture(text_texture, NULL, NULL, &text_rect.w, &text_rect.h);
                        SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
                        SDL_DestroyTexture(text_texture);
                    }

                    if(engine_state.colliders_visible && current->entity->collider != NULL){
                        // paint the collider
                        SDL_Rect collider_rect = ye_convert_rectf_rect(current->entity->collider->rect);
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
    if(engine_state.editor_mode && engine_state.scene_camera_bounds_visible && engine_runtime_state.scene_default_camera != NULL){
        // draw box around viewport of engine_runtime_state.scene_default_camera
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect scene_camera_rect = ye_convert_rectf_rect(engine_runtime_state.scene_default_camera->transform->rect);
        scene_camera_rect.x = scene_camera_rect.x - camera_rect.x;
        scene_camera_rect.y = scene_camera_rect.y - camera_rect.y;
        scene_camera_rect.w = engine_runtime_state.scene_default_camera->camera->view_field.w;
        scene_camera_rect.h = engine_runtime_state.scene_default_camera->camera->view_field.h;
        SDL_RenderDrawRect(renderer, &scene_camera_rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }

    if(engine_state.editor_mode && engine_runtime_state.selected_entity != NULL){
        // draw a pink rect around the selected entity rect
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
        SDL_Rect selected_entity_rect = ye_convert_rectf_rect(engine_runtime_state.selected_entity->transform->rect);
        selected_entity_rect.x = selected_entity_rect.x - camera_rect.x;
        selected_entity_rect.y = selected_entity_rect.y - camera_rect.y;
        SDL_RenderDrawRect(renderer, &selected_entity_rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }
}