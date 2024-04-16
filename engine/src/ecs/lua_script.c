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

#include <stdbool.h>

#include <lua.h>

#include <yoyoengine/yep.h>
#include <yoyoengine/tricks.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/logging.h>
#include <yoyoengine/lua_api.h>
#include <yoyoengine/ecs/lua_script.h>

// marcro for getting the state
// #define ls_state entity->lua_script->state

/*
    Helper functions
*/

bool _run_script(lua_State *state, char *script){
    // if (luaL_loadfile(state, path) || lua_pcall(state, 0, 0, 0)) {
    //     /*
    //         We have failed to load and run the script, log the error
    //         and then disable this script component and cleanup.
    //     */
    //     ye_logf(error,"Error bootstrapping lua script: %s\n", lua_tostring(state, -1));
    //     return false;
    // }
    // return true;

    // run script from string
    if (luaL_dostring(state, script) != LUA_OK) {
        /*
            We have failed to load and run the script, log the error
            and then disable this script component and cleanup.
        */
        ye_logf(error,"Error bootstrapping lua script: %s\n", lua_tostring(state, -1));
        return false;
    }
    return true;
}

void _extract_signature(struct ye_component_lua_script *script, const char *funcName, bool *hasRef) {
    lua_State *L = script->state;

    lua_getglobal(L, funcName);
    if (lua_isfunction(L, -1)) {
        // ye_logf(debug,"Found function %s in script\n", funcName);
        *hasRef = true;
    } else {
        // ye_logf(debug,"Did not find function %s in script\n", funcName);
        *hasRef = false;
    }
    lua_pop(L, 1); // Pop the function or nil value from the stack
}

// really shitty and weird utility function to grab some lua script into a buffer
char* _retrieve_script(const char* handle, bool is_resources_yep) {
    char *data = NULL;
    if(YE_STATE.editor.editor_mode){
        char *full_path = ye_path_resources(handle);
        FILE *file = fopen(full_path, "r");
        if(file == NULL){
            ye_logf(error,"Failed to open file %s\n", full_path);
            return NULL;
        }
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);
        data = malloc(length + 1);
        fread(data, 1, length, file);
        fclose(file);
        data[length] = '\0';
    }
    else{
        struct yep_data_info d;

        if(is_resources_yep)
            d = yep_resource_misc(handle);
        else
            d = yep_engine_resource_misc(handle);
        
        if(d.data == NULL){
            ye_logf(error,"Failed to open file %s\n", handle);
            return NULL;
        }
        data = malloc(d.size + 1);
        memcpy(data, d.data, d.size);
        data[d.size] = '\0';
        free(d.data);
    }
    return data;
}

bool ye_add_lua_script_component(struct ye_entity *entity, const char *handle){
    // ye_logf(debug,"Adding lua script component to entity %s\n", entity->name);
    
    // allocate and assign the component
    entity->lua_script = malloc(sizeof(struct ye_component_lua_script));
    entity->lua_script->active = true;
    
    /*
        Initialize state and load libs
    */
    entity->lua_script->state = luaL_newstate();
    luaL_openlibs(entity->lua_script->state);
    // TODO: we also need to individually register each api function here
    ye_register_lua_scripting_api(entity->lua_script->state);

    // allow game to register lua bindings (function pointer pass state)
    if(YE_STATE.engine.callbacks.register_lua != NULL)
        YE_STATE.engine.callbacks.register_lua(entity->lua_script->state);

    // allow any C tricks to register lua bindings
    ye_register_trick_lua_bindings(entity->lua_script->state);

    // validate state and print errors
    if(entity->lua_script->state == NULL){
        ye_logf(error,"Failed to initialize lua state\n");
        entity->lua_script->active = false;
        free(entity->lua_script);
        entity->lua_script = NULL;
        return false;
    }

    /*
        if we arent in editor, no need to setup the API and actually run the script
        TODO: this could break stuff in the future if you try to add editor run mode
    */
    if(!YE_STATE.editor.editor_mode){
        /*
            Load our engine lua runtime abstractions and run them
            (from engine.yep/runtime.lua)
        */
        char *runtime = _retrieve_script("runtime.lua", false);
        if(runtime == NULL){
            entity->lua_script->active = false;
            free(entity->lua_script);
            entity->lua_script = NULL;
            return false;
        }

        if(!_run_script(entity->lua_script->state, runtime)){
            lua_close(entity->lua_script->state);
            entity->lua_script->active = false;
            entity->lua_script->state = NULL;
            free(entity->lua_script);
            entity->lua_script = NULL;
            return false;
        }

        // ye_logf(debug,"Successfully bootstrapped engine runtime lua script\n");

        /*
            Load the script data into a string
        */
        char *script_data = _retrieve_script(handle, true);
        if(script_data == NULL){
            entity->lua_script->active = false;
            free(entity->lua_script);
            entity->lua_script = NULL;
            return false;
        }

        // TEMP: print the script data
        // ye_logf(debug,"Script data:\n%s\n", script_data);

        /*
            Load our script into the lua state, which will inherently run it
        */
        if(!_run_script(entity->lua_script->state, script_data)){
            lua_close(entity->lua_script->state);
            entity->lua_script->active = false;
            entity->lua_script->state = NULL;
            free(entity->lua_script);
            entity->lua_script = NULL;
            return false;
        }

        /*
            Look through the file and interpret what functions exist in this script
            ex: on_mount, on_update, on_trigger_enter, etc and assign struct fields
        */
        _extract_signature(entity->lua_script, "onMount", &(entity->lua_script->has_on_mount));
        _extract_signature(entity->lua_script, "onUpdate", &(entity->lua_script->has_on_update));
        _extract_signature(entity->lua_script, "onUnmount", &(entity->lua_script->has_on_unmount));
    }


    /*
        call the lua scripts on_mount function in its state

        ALSO if we are not in editor, dont really want to run scripts in editor mode
    */
    if(entity->lua_script->has_on_mount && !YE_STATE.editor.editor_mode){
        ye_run_lua_on_mount(entity->lua_script);
    }

    // doing this at the end so i dont have to free for every error case
    entity->lua_script->script_handle = strdup(handle);

    // add to the lua_script list
    ye_entity_list_add(&lua_script_list_head, entity);

    // ye_logf(debug,"Successfully added lua script component to entity %s\n", entity->name);

    return true;
}

void ye_remove_lua_script_component(struct ye_entity *entity){
    if(entity->lua_script == NULL){
        ye_logf(warning,"Attempted to remove lua script from entity that does not have one\n");
        return;
    }

    if(entity->lua_script->state == NULL){
        ye_logf(warning,"Attempted to remove lua script from entity that does not have a state\n");
        return;
    }

    // run the unmount function
    ye_run_lua_on_unmount(entity->lua_script);

    // shut down the state
    lua_close(entity->lua_script->state);
    entity->lua_script->state = NULL;

    // free the allocated memory
    free(entity->lua_script);
    entity->lua_script = NULL;

    // remove from the lua_script list
    ye_entity_list_remove(&lua_script_list_head, entity);

    // printf("Removed lua script component from entity %s\n", entity->name);
}

void ye_system_lua_scripting(){
    struct ye_entity_node *current = lua_script_list_head;
    while(current != NULL){
        if(current->entity->lua_script->active){
            // run the update function
            ye_run_lua_on_update(current->entity->lua_script);
        }
        current = current->next;
    }
}