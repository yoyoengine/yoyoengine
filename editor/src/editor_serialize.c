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

#include "editor.h"
#include <yoyoengine/yoyoengine.h>

/*
    Begin helper methods to serialize different type of objects into json_t

    Their data is already fully initialized in structs, so we dont have to
    do nearly as much error checking as if we were deserializing

    NOTE: maybe worth doing a few checks to not write any zero or unintialized fields to save space?
    Counter: premature optimization is the root of all evil
*/

/*
    Serialize a transform

    NOTE TODO: we are converting a float to an int to serialize back into a float from an int. pepega
*/
void serialize_entity_transform(struct ye_entity *entity, json_t *entity_json){
    // create the transform object
    json_t *transform = json_object();
    
    // set the x
    json_object_set_new(transform, "x", json_integer((int)entity->transform->x));

    // set the y
    json_object_set_new(transform, "y", json_integer((int)entity->transform->y));

    // set the transform object
    json_object_set_new(entity_json, "transform", transform);
}

void serialize_entity_camera(struct ye_entity *entity, json_t *entity_json){
    // create the camera object
    json_t *camera = json_object();

    // set the active state
    json_object_set_new(camera, "active", json_boolean(entity->camera->active));

    // set the view field object
    json_t *view_field = json_object();
    json_object_set_new(view_field, "w", json_integer(entity->camera->view_field.w));
    json_object_set_new(view_field, "h", json_integer(entity->camera->view_field.h));

    // set the camera object
    json_object_set_new(entity_json, "camera", camera);

    // set the view field object
    json_object_set_new(camera, "view_field", view_field);
}

/*
    NOTE TODO: same thing here, pepega
*/
void serialize_entity_position(struct ye_rectf *position, json_t *parent){
    // create the position object
    json_t *position_json = json_object();

    // set the x
    json_object_set_new(position_json, "x", json_integer((int)position->x));

    // set the y
    json_object_set_new(position_json, "y", json_integer((int)position->y));

    // set the w
    json_object_set_new(position_json, "w", json_integer((int)position->w));

    // set the h
    json_object_set_new(position_json, "h", json_integer((int)position->h));

    // set the position object
    json_object_set_new(parent, "position", position_json);
}

void serialize_entity_renderer(struct ye_entity *entity, json_t *entity_json){
    // create the renderer object
    json_t *renderer = json_object();

    // set the active state
    json_object_set_new(renderer, "active", json_boolean(entity->renderer->active));

    // set the type integer
    json_object_set_new(renderer, "type", json_integer(entity->renderer->type));

    // set the flip booleans
    json_object_set_new(renderer, "flip x", json_boolean(entity->renderer->flipped_x));
    json_object_set_new(renderer, "flip y", json_boolean(entity->renderer->flipped_y));

    // set the z layer
    json_object_set_new(renderer, "z", json_integer(entity->renderer->z));

    // set the alignment
    json_object_set_new(renderer, "alignment", json_integer(entity->renderer->alignment));

    // set the position object
    serialize_entity_position(&entity->renderer->rect, renderer);

    // and now the fun part... the renderer specific impl

    // create impl object
    json_t *impl = json_object();

    switch(entity->renderer->type){
        case YE_RENDERER_TYPE_IMAGE:
            // set the src
            json_object_set_new(impl, "src", json_string(entity->renderer->renderer_impl.image->src));
            break;
        case YE_RENDERER_TYPE_TEXT:
            // set the text
            json_object_set_new(impl, "text", json_string(entity->renderer->renderer_impl.text->text));
            /*
                SAME THING BUT WITH FONTS TOO:

                TODO: actually interesting problem... when we lookup a color, we lose its name when assigning to text.
            
                Potential solutions:
                - store the color name in the impl field
                - write a reverse color lookup in cache function

                Im leaning towards the second, but for now lets just not give a shit :3
            */
            json_object_set_new(impl, "color", json_string("TODO! :3"));
            json_object_set_new(impl, "font", json_string("TODO! :3"));

            break;
        case YE_RENDERER_TYPE_TEXT_OUTLINED:
            // set the text
            json_object_set_new(impl, "text", json_string(entity->renderer->renderer_impl.text_outlined->text));

            // set the outline size
            json_object_set_new(impl, "outline size", json_integer(entity->renderer->renderer_impl.text_outlined->outline_size));

            /*
                We run into the same font and color issue here as above- :3
            */
            json_object_set_new(impl, "color", json_string("TODO! :3"));
            json_object_set_new(impl, "font", json_string("TODO! :3"));
            json_object_set_new(impl, "outline color", json_string("TODO! :3"));

            break;
        case YE_RENDERER_TYPE_ANIMATION:
            // set the animation path
            json_object_set_new(impl, "animation path", json_string(entity->renderer->renderer_impl.animation->animation_path));

            // set the image format
            json_object_set_new(impl, "image format", json_string(entity->renderer->renderer_impl.animation->image_format));

            // set the frame count
            json_object_set_new(impl, "frame count", json_integer(entity->renderer->renderer_impl.animation->frame_count));
            
            // set the frame delay
            json_object_set_new(impl, "frame delay", json_integer(entity->renderer->renderer_impl.animation->frame_delay));

            // set the loops
            json_object_set_new(impl, "loops", json_integer(entity->renderer->renderer_impl.animation->loops));
            
            // set the paused state
            json_object_set_new(impl, "paused", json_boolean(entity->renderer->renderer_impl.animation->paused));

            break;
        default:
            ye_logf(warning, "ermmm... this shouldnt have happend!");
    }

    // set the renderer object
    json_object_set_new(entity_json, "renderer", renderer);

    // set the impl object
    json_object_set_new(renderer, "impl", impl);
}

void serialize_entity_physics(struct ye_entity *entity, json_t *entity_json){
    // create the physics object
    json_t *physics = json_object();

    // set the active state
    json_object_set_new(physics, "active", json_boolean(entity->physics->active));

    // create the velocity object
    json_t *velocity = json_object();

    // set the x
    json_object_set_new(velocity, "x", json_real(entity->physics->velocity.x));

    // set the y
    json_object_set_new(velocity, "y", json_real(entity->physics->velocity.y));

    // set the rotational velocity
    json_object_set_new(physics, "rotational velocity", json_real(entity->physics->rotational_velocity));

    // add the velocity object to the physics object
    json_object_set_new(physics, "velocity", velocity);

    // add the physics object to the entity json
    json_object_set_new(entity_json, "physics", physics);
}

void serialize_entity_collider(struct ye_entity *entity, json_t *entity_json){
    // create the collider object
    json_t *collider = json_object();

    // set the active state
    json_object_set_new(collider, "active", json_boolean(entity->collider->active));

    // set the position
    serialize_entity_position(&entity->collider->rect, collider);

    // set the is trigger
    json_object_set_new(collider, "is trigger", json_boolean(entity->collider->is_trigger));

    // set the relativity
    json_object_set_new(collider, "relative", json_boolean(entity->collider->relative));

    // add the collider object to the entity json
    json_object_set_new(entity_json, "collider", collider);
}

void serialize_entity_tag(struct ye_entity *entity, json_t *entity_json){
    // create the tag object
    json_t *tag = json_object();

    // set the active state
    json_object_set_new(tag, "active", json_boolean(entity->tag->active));

    // set the tags array
    json_t *tags = json_array();

    // for each tag in the entity, add it to the tags array
    for(int i = 0; i < YE_TAG_MAX_NUMBER; i++){
        // if the tag is empty, dont add it
        if(entity->tag->tags[i][0] == '\0'){
            continue;
        }

        json_array_append_new(tags, json_string(entity->tag->tags[i]));
    }

    // add the tags array to the tag object
    json_object_set_new(tag, "tags", tags);

    // add the tag object to the entity json
    json_object_set_new(entity_json, "tag", tag);
}

void editor_write_scene_to_disk(const char *path){
    ye_logf(info,"Writing scene to disk at %s\n", path);
    // load the scene file into a json_t
    json_t *scene = ye_json_read(YE_STATE.runtime.scene_file_path);

    // create a json_t array listing all entities in the scene
    json_t *entities = json_array();
    // for(int i = 0; i < YE_STATE.runtime.entity_count; i++){
    //     json_array_append_new(entities, json_string("meow!"));
    // }

    // lets traverse the entity list and write each entity to the json_t array
    struct ye_entity_node *node = entity_list_head;
    while(node->next != NULL){
        struct ye_entity *entity = node->entity;

        // make sure we exclude editor objects
        if(entity == editor_camera || entity == origin){
            node = node->next;
            continue;
        }


        json_t *entity_json = json_object();

        // set the name
        json_object_set_new(entity_json, "name", json_string(entity->name));

        // set the active status
        json_object_set_new(entity_json, "active", json_boolean(entity->active));

        // create the components object
        json_object_set_new(entity_json, "components", json_object());

        if(entity->transform != NULL){
            serialize_entity_transform(entity, json_object_get(entity_json, "components"));
        }

        if(entity->camera != NULL){
            serialize_entity_camera(entity, json_object_get(entity_json, "components"));
        }

        if(entity->renderer != NULL){
            serialize_entity_renderer(entity, json_object_get(entity_json, "components"));
        }

        if(entity->physics != NULL){
            serialize_entity_physics(entity, json_object_get(entity_json, "components"));
        }

        if(entity->collider != NULL){
            serialize_entity_collider(entity, json_object_get(entity_json, "components"));
        }

        if(entity->tag != NULL){
            serialize_entity_tag(entity, json_object_get(entity_json, "components"));
        }

        // add the entity to the entity json array
        json_array_append_new(entities, entity_json);

        node = node->next;
    }

    // update the scene file with the new entity list
    json_object_set_new(json_object_get(scene, "scene"), "entities", entities);

    // ye_json_log(scene); //TODO: figure out how we update the name version styles and prefabs

    // write the scene file
    ye_json_write(YE_STATE.runtime.scene_file_path, scene);                      
}