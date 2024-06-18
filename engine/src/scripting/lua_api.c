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

#include <string.h>
#include <stdlib.h>

#include <lua.h>

#include <yoyoengine/scene.h>
#include <yoyoengine/logging.h>
#include <yoyoengine/lua_api.h>
#include <yoyoengine/ecs/renderer.h>

// remove component
#include <yoyoengine/ecs/camera.h>
#include <yoyoengine/ecs/button.h>
#include <yoyoengine/ecs/lua_script.h>
#include <yoyoengine/ecs/physics.h>
#include <yoyoengine/ecs/collider.h>
#include <yoyoengine/ecs/transform.h>
#include <yoyoengine/ecs/renderer.h>
#include <yoyoengine/ecs/audiosource.h>
#include <yoyoengine/ecs/tag.h>

/*
    Function that allows lua to log using the engine logger
*/
int lua_log(lua_State* L){
    // get the function name
    const char* level = lua_tostring(L, 1);
    const char* message = lua_tostring(L, 2);
    // strcat(message, "\n");

    // begin checks for functions
    if(strcmp(level, "debug") == 0){
        _ye_lua_logf(debug, message);
    }
    if(strcmp(level, "info") == 0){
        _ye_lua_logf(info, message);
    }
    if(strcmp(level, "warn") == 0){
        _ye_lua_logf(warning, message);
    }
    if(strcmp(level, "error") == 0){
        _ye_lua_logf(error, message);
    }

    return 0;
}

int lua_load_scene(lua_State* L){ // TODO: totally not working
    const char* scene = lua_tostring(L, 1);
    ye_load_scene_deferred(scene);
}

int lua_debug_quit(lua_State* L){ // TODO: removeme
    exit(0);
    return 0;
}

int ye_lua_check_component_exists(lua_State* L){
    struct ye_entity * entity = lua_touserdata(L, 1);

    // check entity exists
    if(entity == NULL){
        ye_logf(error, "Tried to check validity of component on null entity.\n");
        lua_pushboolean(L, 0);
        return 1;
    }

    int comp_indx = lua_tointeger(L, 2);
    void* components[] = {
        entity->transform,
        entity->renderer,
        entity->camera,
        entity->lua_script,
        entity->button,
        entity->physics,
        entity->collider,
        entity->tag,
        entity->audiosource
    };
    int num_components = sizeof(components) / sizeof(components[0]);

    if(comp_indx >= 0 && comp_indx < num_components){
        lua_pushboolean(L, components[comp_indx] != NULL);
    } else {
        lua_pushboolean(L, 0);
    }
    return 1;
}

int ye_lua_check_renderer_component_type_exists(lua_State* L){
    struct ye_entity * entity = lua_touserdata(L, 1);

    // check entity exists
    if(entity == NULL){
        ye_logf(error, "Tried to check existance of renderer type on null entity.\n");
        lua_pushboolean(L, 0);
        return 1;
    }

    int type_indx = lua_tointeger(L, 2);

    if(entity->renderer == NULL){
        lua_pushboolean(L, 0);
        ye_logf(error, "Tried to check validity of renderer component on entity with no renderer.\n");
        return 1;
    }

    if(entity->renderer->type == type_indx){
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }

    return 1;
}



int ye_lua_remove_component(lua_State* L){
    struct ye_entity * entity = lua_touserdata(L, 1);

    // check entity exists
    if(entity == NULL){
        ye_logf(error, "Tried to remove component on null entity.\n");
        return 0;
    }

    int comp_indx = lua_tointeger(L, 2);
    
    switch(comp_indx){
        case 0:
            ye_remove_transform_component(entity);
            break;
        case 1:
            ye_remove_renderer_component(entity);
            break;
        case 2:
            ye_remove_camera_component(entity);
            break;
        case 3:
            ye_remove_lua_script_component(entity);
            break;
        case 4:
            ye_remove_button_component(entity);
            break;
        case 5:
            ye_remove_physics_component(entity);
            break;
        case 6:
            ye_remove_collider_component(entity);
            break;
        case 7:
            ye_remove_tag_component(entity);
            break;
        case 8:
            ye_remove_audiosource_component(entity);
            break;
        default:
            ye_logf(error, "Tried to remove invalid component type.\n");
            break;
    }

    return 0;
}



/*
    Reach out to all the decentralized api
    files and call their register functions
*/
void ye_register_lua_scripting_api(lua_State *state){
    // scattered fns
    lua_register(state, "log", lua_log);
    lua_register(state, "loadScene", lua_load_scene);
    // lua_register(state, "ye_debug_quit", lua_debug_quit);

    /*
        Entity
    */
    ye_lua_entity_register(state);

    /*
        Transform
    */
    ye_lua_transform_register(state);

    /*
        Camera
    */
    ye_lua_camera_register(state);

    /*
        Renderer
    */
    ye_lua_renderer_register(state);

    /*
        Button
    */
    ye_lua_button_register(state);

    /*
        Tag
    */
    ye_lua_tag_register(state);

    /*
        Collider
    */
    ye_lua_collider_register(state);

    /*
        Physics
    */
    ye_lua_physics_register(state);

    /*
        LuaScript
    */
    ye_lua_lua_script_register(state);

    // check if component exists
    lua_register(state, "ye_lua_check_component_exists", ye_lua_check_component_exists);

    // check if renderer component type exists
    lua_register(state, "ye_lua_check_renderer_component_type_exists", ye_lua_check_renderer_component_type_exists);

    // cross state function
    lua_register(state, "ye_invoke_cross_state_function", ye_invoke_cross_state_function);

    // cross state value
    lua_register(state, "ye_write_cross_state_value", ye_write_cross_state_value);
    lua_register(state, "ye_read_cross_state_value", ye_read_cross_state_value);

    // remove component
    lua_register(state, "ye_lua_remove_component", ye_lua_remove_component);

    /*
        Subsystems
    */
    ye_lua_audio_register(state);
}