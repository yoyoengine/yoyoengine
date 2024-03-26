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
#include <stdlib.h>
#include <stdbool.h>

#include <jansson.h>

#include <yoyoengine/yep.h>
#include <yoyoengine/json.h>
#include <yoyoengine/scene.h>
#include <yoyoengine/cache.h>
#include <yoyoengine/audio.h>
#include <yoyoengine/utils.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/ecs/tag.h>
#include <yoyoengine/ecs/camera.h>
#include <yoyoengine/ecs/button.h>
#include <yoyoengine/ecs/physics.h>
#include <yoyoengine/ecs/collider.h>
#include <yoyoengine/ecs/renderer.h>
#include <yoyoengine/ecs/transform.h>
#include <yoyoengine/ecs/lua_script.h>
#include <yoyoengine/debug_renderer.h>
#include <yoyoengine/ecs/audiosource.h>


void ye_init_scene_manager(){
    YE_STATE.runtime.scene_name = NULL;
    YE_STATE.runtime.scene_file_path = NULL;
}

/*
    TODO: cut down on code duplication by doing the following:
    - repetitive fields like xywh, active could be done in a single function
*/

/*
    ===================================================================
    REALLY BORING REPETITIVE COMPONENT CONSTRUCTION FUNCTIONS:
    literally just validating fields and calling component constructors
    ===================================================================
*/

struct ye_rectf ye_retrieve_position(json_t *parent){
    // validate position field
    json_t *position = NULL;
    struct ye_rectf b;
    if(!ye_json_object(parent,"position",&position)) {
        ye_logf(warning,"Entity %s has a transform component, but it is missing the position field\n");
        return (struct ye_rectf){0,0,0,0};
    } else {
        int x,y,w,h;
        if(!ye_json_int(position,"x",&x) || !ye_json_int(position,"y",&y) || !ye_json_int(position,"w",&w) || !ye_json_int(position,"h",&h)) {
            ye_logf(warning,"Entity %s has a transform component with invalid position field\n");
            b = (struct ye_rectf){0,0,0,0};
        } else {
            return (struct ye_rectf){
                .x = (float)x,
                .y = (float)y,
                .w = (float)w,
                .h = (float)h
            };
        }
    }
    return b;
}

void ye_construct_transform(struct ye_entity* e, json_t* transform, const char* entity_name) {
    int x, y;
    if(!ye_json_int(transform,"x",&x) || !ye_json_int(transform,"y",&y)) {
        ye_logf(warning,"Entity %s has a transform component with invalid position field\n", entity_name);
        x = 0;
        y = 0;
    }

    // construct transform component
    ye_add_transform_component(e,x,y); 
}

void ye_construct_camera(struct ye_entity* e, json_t* camera, const char* entity_name){
    // validate the view field
    json_t *view_field = NULL;
    if(!ye_json_object(camera,"view field",&view_field)) {
        ye_logf(warning,"Entity %s has a camera component, but it is missing the view field\n", entity_name);
        return;
    }

    // validate the x,y,w,h ints in view feild
    int cx,cy,cw,ch;
    if(!ye_json_int(view_field,"x",&cx) || !ye_json_int(view_field,"y",&cy) || !ye_json_int(view_field,"w",&cw) || !ye_json_int(view_field,"h",&ch)) {
        ye_logf(warning,"Entity %s has a camera component with invalid view field\n", entity_name);
        return;
    }

    int z;
    if(!ye_json_int(camera,"z",&z)) {
        ye_logf(warning,"Entity %s has a camera component, but it is missing the z field\n", entity_name);
        z = 999;
    }

    // add the camera component
    ye_add_camera_component(e,z,(SDL_Rect){cx,cy,cw,ch});

    // update active state
    if(ye_json_has_key(camera,"active")){
        bool active = true;    ye_json_bool(camera,"active",&active);
        e->camera->active = active;
    }
}

void ye_construct_renderer(struct ye_entity* e, json_t* renderer, const char* entity_name){
    
    // get the type of renderer
    int type_int;
    if(!ye_json_int(renderer,"type",&type_int)) {
        ye_logf(warning,"Entity %s has a renderer component, but it is missing the type field\n", entity_name);
        return;
    }

    enum ye_component_renderer_type type = (enum ye_component_renderer_type)type_int;

    // get the impl field
    json_t *impl = NULL;
    if(!ye_json_object(renderer,"impl",&impl)) {
        ye_logf(warning,"Entity %s has a renderer component, but it is missing the impl field\n", entity_name);
        return;
    }

    int z;
    if(!ye_json_int(renderer,"z",&z)) {
        ye_logf(warning,"Entity %s has a renderer component, but it is missing the z field\n", entity_name);
        z = 999;
    }

    struct ye_rectf rect = ye_retrieve_position(renderer);

    const char *animation_path = NULL;  // comply with mingw & clang
    const char *_text = NULL;           // comply with mingw & clang
    const char *src = NULL;             // comply with mingw & clang
    const char *text = NULL;            // comply with mingw & clang
    const char *font = NULL;            // comply with mingw & clang
    const char *color = NULL;           // comply with mingw & clang
    switch(type){
        // ye_logf(info,"Constructing renderer: %s\n", entity_name);
        case YE_RENDERER_TYPE_IMAGE:

            // set src to impl->src
            if(!ye_json_string(impl,"src",&src)) {
                ye_logf(warning,"Entity \"%s\" has a renderer component, but it is missing the impl->src field\n", entity_name);
                return;
            }

            /*
                we no longer need to hack around saving relative not full path:
                now that we have binary format for all resources its src is just a handle
            */
            ye_add_image_renderer_component(e,z,src);
            break;
        case YE_RENDERER_TYPE_TEXT:
            // get the text field
            if(!ye_json_string(impl,"text",&text)) {
                ye_logf(warning,"Entity \"%s\" has a renderer component, but it is missing the text field\n", entity_name);
                return;
            }

            // get the font field
            if(!ye_json_string(impl,"font",&font)) {
                ye_logf(warning,"Entity \"%s\" has a renderer component, but it is missing the font field\n", entity_name);
                return;
            }

            // get the color field
            if(!ye_json_string(impl,"color",&color)) {
                ye_logf(warning,"Entity \"%s\" has a renderer component, but it is missing the color field\n", entity_name);
                return;
            }

            // get the font_size field
            int font_size;
            if(!ye_json_int(impl,"font_size",&font_size)) {
                ye_logf(warning,"Entity \"%s\" has a renderer component, but it is missing the font_size field. It has been loaded at 16pt\n", entity_name);
                font_size = 16;
            }

            int wrap_width;
            if(!ye_json_int(impl,"wrap_width",&wrap_width)) {
                ye_logf(warning,"Entity \"%s\" has a renderer component, but it is missing the wrap width field\n", entity_name);
                return;
            }

            ye_add_text_renderer_component(e,z,text,font,font_size,color,wrap_width);
            break;
        case YE_RENDERER_TYPE_TEXT_OUTLINED:
            // get the text field
            if(!ye_json_string(impl,"text",&_text)) {
                ye_logf(warning,"Entity \"%s\" has a renderer component, but it is missing the text field\n", entity_name);
                return;
            }

            // get the font field
            const char *_font = NULL;
            if(!ye_json_string(impl,"font",&_font)) {
                ye_logf(warning,"Entity \"%s\" has a renderer component, but it is missing the font field\n", entity_name);
                return;
            }

            // get the font_size field
            int outlined_font_size;
            if(!ye_json_int(impl,"font_size",&outlined_font_size)) {
                ye_logf(warning,"Entity \"%s\" has a renderer component, but it is missing the font_size field. It has been loaded at 16pt\n", entity_name);
                outlined_font_size = 16;
            }

            // get the color field
            const char *_color = NULL;
            if(!ye_json_string(impl,"color",&_color)) {
                ye_logf(warning,"Entity \"%s\" has a renderer component, but it is missing the color field\n", entity_name);
                return;
            }

            // get the outline color field
            const char *outline_color = NULL;
            if(!ye_json_string(impl,"outline color",&outline_color)) {
                ye_logf(warning,"Entity \"%s\" has a renderer component, but it is missing the outline color field\n", entity_name);
                return;
            }

            // get the outline size field
            int outline_size;
            if(!ye_json_int(impl,"outline size",&outline_size)) {
                ye_logf(warning,"Entity \"%s\" has a renderer component, but it is missing the outline size field\n", entity_name);
                return;
            }

            int _wrap_width;
            if(!ye_json_int(impl,"wrap_width",&_wrap_width)) {
                ye_logf(warning,"Entity \"%s\" has a renderer component, but it is missing the wrap width field\n", entity_name);
                return;
            }

            ye_add_text_outlined_renderer_component(e,z,_text,_font,outlined_font_size,_color,outline_color,outline_size,_wrap_width);

            break;
        case YE_RENDERER_TYPE_ANIMATION:
            // get the animation path string (path to the meta file)
            if(!ye_json_string(impl,"animation path",&animation_path)) {
                ye_logf(warning,"Entity \"%s\" has a renderer component, but it is missing the animation path field\n", entity_name);
                return;
            }

            ye_add_animation_renderer_component(e,z,animation_path);

            break;
        case YE_RENDERER_TYPE_TILEMAP_TILE:
            // get the handle
            if(!ye_json_string(impl,"handle",&src)) {
                ye_logf(warning,"Entity \"%s\" has a renderer component, but it is missing the handle field\n", entity_name);
                return;
            }

            // get the "position" which is really just the src rect
            SDL_Rect src_rect = ye_convert_rectf_rect(ye_retrieve_position(impl));
            
            ye_add_tilemap_renderer_component(e,z,src,src_rect);
            break;
        default:
            ye_logf(warning,"Entity %s has a renderer component, but it is missing the type field\n", entity_name);
            break;
    }
    
    // set the rect
    e->renderer->rect = rect;

    // update the aplha (if exists)
    if(ye_json_has_key(renderer,"alpha")){
        int alpha = 255;    ye_json_int(renderer,"alpha",&alpha);
        e->renderer->alpha = alpha;
    }

    // check for flipped_x and flipped_y and update
    if(ye_json_has_key(renderer,"flipped_x")){
        bool flipped_x = false;    ye_json_bool(renderer,"flipped_x",&flipped_x);
        e->renderer->flipped_x = flipped_x;
    }
    if(ye_json_has_key(renderer,"flipped_y")){
        bool flipped_y = false;    ye_json_bool(renderer,"flipped_y",&flipped_y);
        e->renderer->flipped_y = flipped_y;
    }

    // update the alignment (if exists)
    enum ye_alignment alignment;
    if(!ye_json_int(renderer,"alignment",(int*)&alignment)) {
        ye_logf(warning,"Entity %s has a renderer component, but it is missing the alignment field\n", entity_name);
        alignment = YE_ALIGN_MID_CENTER;
    }
    // printf("alignment of %s: %d\n", entity_name, alignment);
    e->renderer->alignment = alignment;

    // preserve_size
    bool preserve_size;
    if(!ye_json_bool(renderer,"preserve size",&preserve_size)) {
        ye_logf(warning,"Entity %s has a renderer component, but it is missing the preserve size field\n", entity_name);
        preserve_size = false;
    }
    e->renderer->preserve_original_size = preserve_size;

    // get a rotation if existant and update it
    if(ye_json_has_key(renderer,"rotation")){
        float angle = 0;    ye_json_float(renderer,"rotation",&angle);
        e->renderer->rotation = angle;
    }

    // if "center" exists validate x and y and set them
    if(ye_json_has_key(renderer,"center")){
        json_t *center = NULL;
        if(!ye_json_object(renderer,"center",&center)) {
            ye_logf(warning,"Entity \"%s\" has a renderer component, but it is missing the center field\n", entity_name);
        } else {
            int x,y;
            if(!ye_json_int(center,"x",&x) || !ye_json_int(center,"y",&y)) {
                ye_logf(warning,"Entity %s has a renderer component with invalid center field\n", entity_name);
            } else {
                e->renderer->center = (struct SDL_Point){x,y};
            }
        }
    }

    // update active state
    if(ye_json_has_key(renderer,"active")){
        bool active = true;    ye_json_bool(renderer,"active",&active);
        e->renderer->active = active;
    }
}

void ye_construct_physics(struct ye_entity* e, json_t* physics, const char* entity_name){
    // get velocity
    if(ye_json_has_key(physics,"velocity")){
        json_t *velocity = NULL;
        if(!ye_json_object(physics,"velocity",&velocity)) {
            ye_logf(warning,"Entity \"%s\" has a physics component, but it is missing the velocity field\n", entity_name);
        } else {
            float x,y;
            if(!ye_json_float(velocity,"x",&x) || !ye_json_float(velocity,"y",&y)) {
                ye_logf(warning,"Entity %s has a physics component with invalid velocity field\n", entity_name);
            } else {
                ye_add_physics_component(e,x,y);
            }
        }
    }

    // get rotational velocity
    if(ye_json_has_key(physics,"rotational velocity")){
        float rotational_velocity = 0;    ye_json_float(physics,"rotational velocity",&rotational_velocity);
        e->physics->rotational_velocity = rotational_velocity;
    }

    // update active state
    if(ye_json_has_key(physics,"active")){
        bool active = true;    ye_json_bool(physics,"active",&active);
        e->physics->active = active;
    }
}

void ye_construct_tag(struct ye_entity* e, json_t* tag, const char* entity_name){
    // add tag component
    ye_add_tag_component(e);

    // update active state
    if(ye_json_has_key(tag,"active")){
        bool active = true;    ye_json_bool(tag,"active",&active);
        e->tag->active = active;
    }

    // if tags array exists, add each tag
    json_t *tags = NULL;
    if(ye_json_array(tag,"tags",&tags)) {
        // add each tag in the array
        for(int i = 0; i < json_array_size(tags); i++){
            const char *tag_name = NULL; ye_json_arr_string(tags,i,&tag_name);
            ye_add_tag(e,tag_name);
        }
    }
}

void ye_construct_collider(struct ye_entity* e, json_t* collider, const char* entity_name){
    // validate bounds field
    json_t *bounds = NULL;
    struct ye_rectf b = ye_retrieve_position(collider);

    // validate is_trigger field
    bool is_trigger;
    if(!ye_json_bool(collider,"is trigger",&is_trigger)) {
        ye_logf(warning,"Entity %s has a collider component, but it is missing the is trigger field\n", entity_name);
        is_trigger = false;
    }

    // add the collider component
    if(!is_trigger)
        ye_add_static_collider_component(e,b);
    else
        ye_add_trigger_collider_component(e,b);

    // validate the relative field
    bool relative;
    if(!ye_json_bool(collider,"relative",&relative)) {
        ye_logf(warning,"Entity %s has a collider component, but it is missing the relative field\n", entity_name);
        relative = true;
    }
    e->collider->relative = relative;
        
    // update active state
    if(ye_json_has_key(collider,"active")){
        bool active = true;    ye_json_bool(collider,"active",&active);
        e->collider->active = active;
    }
}

void ye_construct_script(struct ye_entity* e, json_t* script, const char* entity_name){
    // validate script field
    const char *script_path = NULL;
    if(!ye_json_string(script,"handle",&script_path)) {
        ye_logf(warning,"Entity %s has a script component, but it is missing the handle field\n", entity_name);
        return;
    }

    // add the script component
    ye_add_lua_script_component(e,script_path);

    // update active state
    if(ye_json_has_key(script,"active")){
        bool active = true;    ye_json_bool(script,"active",&active);
        e->lua_script->active = active;
    }
}

void ye_construct_audiosource(struct ye_entity* e, json_t* audiosource, const char* entity_name){
    // handle the simulated bool
    bool simulated;
    if(!ye_json_bool(audiosource,"simulated",&simulated)) {
        ye_logf(warning,"Entity %s has an audiosource component, but it is missing the simulated field\n", entity_name);
        simulated = false;
    }
    
    // validate src (handle) field
    const char *handle = NULL;
    if(!ye_json_string(audiosource,"src",&handle)) {
        ye_logf(warning,"Entity %s has an audiosource component, but it is missing the src field\n", entity_name);
        return;
    }

    // position field
    struct ye_rectf b = ye_retrieve_position(audiosource);

    // play on awake bool
    bool play_on_awake;
    if(!ye_json_bool(audiosource,"play on awake",&play_on_awake)) {
        ye_logf(warning,"Entity %s has an audiosource component, but it is missing the \"play on awake\" field\n", entity_name);
        play_on_awake = false;
    }

    // volume float 0-1
    float volume;
    if(!ye_json_float(audiosource,"volume",&volume)) {
        ye_logf(warning,"Entity %s has an audiosource component, but it is missing the volume field\n", entity_name);
        volume = 1.0f;
    }

    // loops int
    int loops;
    if(!ye_json_int(audiosource,"loops",&loops)) {
        ye_logf(warning,"Entity %s has an audiosource component, but it is missing the loops field\n", entity_name);
        loops = 0;
    }

    // add the audiosource component
    ye_add_audiosource_component(e,handle,volume,play_on_awake,loops,simulated,b);

    // update active state
    if(ye_json_has_key(audiosource,"active")){
        bool active = true;    ye_json_bool(audiosource,"active",&active);
        e->audiosource->active = active;
    }

    // update the relative field
    if(ye_json_has_key(audiosource,"relative")){
        bool relative = true;    ye_json_bool(audiosource,"relative",&relative);
        e->audiosource->relative = relative;
    }
}

void ye_construct_button(struct ye_entity* e, json_t* button, const char* entity_name){
    // validate the position field
    struct ye_rectf b = ye_retrieve_position(button);

    // add the button component
    ye_add_button_component(e,b);

    // update active state
    if(ye_json_has_key(button,"active")){
        bool active = true;    ye_json_bool(button,"active",&active);
        e->button->active = active;
    }

    // update the relative field
    if(ye_json_has_key(button,"relative")){
        bool relative = true;    ye_json_bool(button,"relative",&relative);
        e->button->relative = relative;
    }
}

/*
    ===================================================================
    ===================================================================
*/

void ye_construct_scene(json_t *entities){
    /*
        traverse backwards (serialization is traversing LL,
        so we need to reverse it to keep the same order)
    */
    for(int i = json_array_size(entities) - 1; i >= 0; i--){
        json_t *entity = NULL;      ye_json_arr_object(entities,i,&entity);    
        
        // get entity name
        const char *entity_name = NULL;   ye_json_string(entity,"name",&entity_name);
        struct ye_entity *e = NULL;
        if(entity_name == NULL){
            ye_logf(warning,"Unnamed entity in scene file. It's name will be automatically assigned.\n");
            e = ye_create_entity();
        }
        else{
            // ye_logf(info,"Constructing entity: %s\n", entity_name);
            e = ye_create_entity_named(entity_name);
        }

        if(entity == NULL){
            ye_logf(error,"Failed to construct entity.\n");
            continue;
        }

        // set entities properties
        if(ye_json_has_key(entity,"active")){
            bool active = true;    ye_json_bool(entity,"active",&active);
            e->active = active;
        }

        // check if components exist
        json_t *components = NULL; ye_json_object(entity,"components",&components);

        // if we have transform on entity
        if(ye_json_has_key(components,"transform")){
            json_t *transform = NULL; ye_json_object(components,"transform",&transform);
            ye_construct_transform(e,transform,entity_name);
        }

        // if we have camera on entity
        if(ye_json_has_key(components,"camera")){
            json_t *camera = NULL; ye_json_object(components,"camera",&camera);
            if(camera == NULL){
                ye_logf(warning,"Entity %s has a renderer field, but it's invalid.\n", entity_name);
                continue;
            }
            ye_construct_camera(e,camera,entity_name);
        }
        
        // if we have a renderer on our entity
        if(ye_json_has_key(components,"renderer")){
            json_t *renderer = NULL; ye_json_object(components,"renderer",&renderer);
            if(renderer == NULL){
                ye_logf(warning,"Entity %s has a renderer field, but it's invalid.\n", entity_name);
                continue;
            }
            ye_construct_renderer(e,renderer,entity_name);
        }

        // if we have a physics component on our entity
        if(ye_json_has_key(components,"physics")){
            json_t *physics = NULL; ye_json_object(components,"physics",&physics);
            if(physics == NULL){
                ye_logf(warning,"Entity %s has a physics field, but it's invalid.\n", entity_name);
                continue;
            }
            ye_construct_physics(e,physics,entity_name);
        }

        // if we have a tag component on our entity
        if(ye_json_has_key(components,"tag")){
            json_t *tag = NULL; ye_json_object(components,"tag",&tag);
            if(tag == NULL){
                ye_logf(warning,"Entity %s has a tag field, but it's invalid.\n", entity_name);
                continue;
            }
            ye_construct_tag(e,tag,entity_name);
        }

        // if we have a collider component on our entity
        if(ye_json_has_key(components,"collider")){
            json_t *collider = NULL; ye_json_object(components,"collider",&collider);
            if(collider == NULL){
                ye_logf(warning,"Entity %s has a collider field, but it's invalid.\n", entity_name);
                continue;
            }
            ye_construct_collider(e,collider,entity_name);
        }

        // script component
        if(ye_json_has_key(components,"script")){
            json_t *script = NULL; ye_json_object(components,"script",&script);
            if(script == NULL){
                ye_logf(warning,"Entity %s has a script field, but it's invalid.\n", entity_name);
                continue;
            }
            ye_construct_script(e,script,entity_name);
        }

        // if we have an audiosource
        if(ye_json_has_key(components,"audiosource")){
            json_t *audiosource = NULL; ye_json_object(components,"audiosource",&audiosource);
            if(audiosource == NULL){
                ye_logf(warning,"Entity %s has a audiosource field, but it's invalid.\n", entity_name);
                continue;
            }
            ye_construct_audiosource(e,audiosource,entity_name);
        }

        // button comp
        if(ye_json_has_key(components,"button")){
            json_t *button = NULL; ye_json_object(components,"button",&button);
            if(button == NULL){
                ye_logf(warning,"Entity %s has a button field, but it's invalid.\n", entity_name);
                continue;
            }
            ye_construct_button(e,button,entity_name);
        }
    }
}

void ye_load_scene(const char *scene_path){
    // shutdown and restart audio subsystem TODO: do some soft reset instead
    ye_shutdown_audio();

    // wipe the ecs so its ready to be populated (this will destroy and re-create editor entities, but the editor will best effort recreate and attach them)
    ye_purge_ecs();

    // wipe non persistant render entities (additional and debug)
    ye_debug_renderer_cleanup(false);

    ye_init_audio();

    /*
        If we are in editor mode, this scene file will be loaded from the loose resources dir, if runtime it will be packed
    */
    json_t *SCENE = NULL; 
    if(YE_STATE.editor.editor_mode){
        SCENE = ye_json_read(ye_path_resources(scene_path));
    }
    else{
        SCENE = yep_resource_json(scene_path);
    }

    // try to open the scene file
    if(SCENE == NULL){
        ye_logf(error,"Failed to load scene %s\n", scene_path);
        json_decref(SCENE);
        return;
    }

    if(YE_STATE.runtime.scene_file_path != NULL)
        free(YE_STATE.runtime.scene_file_path);
    
    // malloc new string for current scene file path and copy it over
    YE_STATE.runtime.scene_file_path = malloc(strlen(scene_path)+1);
    strcpy(YE_STATE.runtime.scene_file_path,scene_path);

    // read some meta about the scene and check validity
    int scene_version;
    if(!ye_json_int(SCENE, "version", &scene_version)){
        ye_logf(error,"Scene \"%s\" has no version number\n", scene_path);
        json_decref(SCENE);
        return;
    }
    // scene files are backwards compatible (for now) but obviously cant guarantee being forwards compatible
    if(scene_version > YE_ENGINE_SCENE_VERSION){
        ye_logf(error,"Scene \"%s\" has version %d, but the engine only supports up to version %d\n", scene_path, scene_version, YE_ENGINE_SCENE_VERSION);
        json_decref(SCENE);
        return;
    }

    const char *scene_name; 
    if(!ye_json_string(SCENE, "name", &scene_name)){
        ye_logf(warning,"Un-named scene loaded %s\n", scene_path);
    }
    else{
        if(YE_STATE.runtime.scene_name != NULL)
            free(YE_STATE.runtime.scene_name);
        YE_STATE.runtime.scene_name = strdup(scene_name);
        ye_logf(info,"Loaded scene: %s\n", scene_name);
    }

    // pre cache all of its colors, fonts (TODO: thread this?)
    json_t *styles; ye_json_array(SCENE, "styles", &styles);
    // cache each styles file in array
    for(int i = 0; i < json_array_size(styles); i++){
        const char *path; ye_json_arr_string(styles, i, &path);
        ye_pre_cache_styles(path);
    }

    // pre cache all of a scenes assets (TODO: thread this?)
    json_t *scene = NULL; ye_json_object(SCENE, "scene", &scene);
    ye_pre_cache_scene(scene); // lowercase scene is the actual key

    // construct all entities and components
    json_t *entities = NULL;
    ye_json_array(scene,"entities",&entities);
    if(entities == NULL){
        ye_logf(error,"%s","Failed to read entities from scene.\n");
        return;
    }

    // construct scene
    ye_construct_scene(entities);

    // check if the scene has a default camera and set it if so, if not log error
    const char* default_camera_name = NULL;
    if(!ye_json_string(scene,"default camera",&default_camera_name)){
        ye_logf(error,"Scene \"%s\" has no default camera\n", scene_path);
    }
    else{
        struct ye_entity *camera = ye_get_entity_by_name(default_camera_name);
        if(!YE_STATE.editor.editor_mode){
            ye_logf(info,"Setting default camera to: %s\n", default_camera_name);
            if(camera == NULL){
                ye_logf(error,"Scene \"%s\" has no camera named \"%s\"\n", scene_path, default_camera_name);
            }
            else{
                ye_set_camera(camera);
            }
        }
        else{
            YE_STATE.editor.scene_default_camera = camera;
        }
    }

    /*
        since audio is on its own thread, lets start it now that everything else is done
        The expected format of music in the scene file is this:
        "music":{
            "src": "music/2024.mp3",
            "loop": true,
            "volume": 1
        },
    */
    if(!YE_STATE.editor.editor_mode){
        json_t *music = NULL;
        if(ye_json_object(scene,"music",&music)){
            const char *src = NULL; ye_json_string(music,"src",&src);
            bool loop = false; ye_json_bool(music,"loop",&loop);
            float volume = 1; ye_json_float(music,"volume",&volume);

            int loops = 0;
            if(loop)
                loops = -1;

            ye_play_music(src,loops,volume);
        }
    }

    // deref the scene file.
    json_decref(SCENE);

    // send a scene loaded callback
    if(YE_STATE.engine.callbacks.scene_load != NULL)
        YE_STATE.engine.callbacks.scene_load(YE_STATE.runtime.scene_name);
}

char *ye_get_scene_name(){
    return YE_STATE.runtime.scene_name;
}

void ye_shutdown_scene_manager(){
    if(YE_STATE.runtime.scene_name != NULL)
        free(YE_STATE.runtime.scene_name);
}

void ye_reload_scene(){
    if(YE_STATE.runtime.scene_name == NULL || YE_STATE.runtime.scene_file_path == NULL){
        ye_logf(error,"%s","No scene loaded to reload.\n");
        return;
    }

    // we cant pass the malloc path beacuse it will destroy it while it tries to recreate it from bad memory.
    // create a temp malloced copy of the file path
    char *temp = malloc(strlen(YE_STATE.runtime.scene_file_path)+1);
    strcpy(temp,YE_STATE.runtime.scene_file_path);

    ye_load_scene(temp);

    // free the temp copy
    free(temp);
}

char *deferred_scene_handle = NULL;

void ye_load_scene_deferred(const char *scene_path){
    if(deferred_scene_handle != NULL)
        free(deferred_scene_handle);
    deferred_scene_handle = strdup(scene_path);
}

bool ye_scene_check_deferred_load(){
    if(deferred_scene_handle != NULL){
        ye_load_scene(deferred_scene_handle);
        free(deferred_scene_handle);
        deferred_scene_handle = NULL;
        return true;
    }
    return false;
}