/*
    This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

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

#include <lua.h>

#include <yoyoengine/ecs/renderer.h>
#include <yoyoengine/lua_api.h>
#include <yoyoengine/logging.h>




/*
    Shaping:
    this is going to be really complex, because the renderer component
    in ecs is essentially a huge nested complex beast with a protected
    impl field. we will probably need multiple query construct and modify
    for non global renderer fields... should figure out lua access first
*/


int ye_lua_create_image_renderer(lua_State *L) {
    ye_logf(debug, "creating image renderer\n");
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not create renderer: entity is null\n");
        return 0;
    }

    // get path, z
    const char * path = luaL_checkstring(L, 2);
    int z = luaL_checknumber(L, 3);

    ye_logf(debug, "adding image renderer comp\n");
    ye_add_image_renderer_component(ent, z, path);

    return 0;
}



/*
    Pushes to stack (in order):
    - isActive
    - isRelative
    - alpha
    - z
    - x
    - y
    - w
    - h
    - alignment
    - rotation
    - flipX
    - flipY
    - preserveOriginalSize
    - type
*/
int ye_lua_renderer_query(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not query renderer: entity is null\n");
        return 0;
    }

    lua_pushboolean(L, ent->renderer->active);
    lua_pushboolean(L, ent->renderer->relative);
    lua_pushnumber(L, ent->renderer->alpha);
    lua_pushnumber(L, ent->renderer->z);
    lua_pushnumber(L, ent->renderer->rect.x);
    lua_pushnumber(L, ent->renderer->rect.y);
    lua_pushnumber(L, ent->renderer->rect.w);
    lua_pushnumber(L, ent->renderer->rect.h);
    lua_pushnumber(L, ent->renderer->alignment);
    lua_pushnumber(L, ent->renderer->rotation);
    lua_pushboolean(L, ent->renderer->flipped_x);
    lua_pushboolean(L, ent->renderer->flipped_y);
    lua_pushboolean(L, ent->renderer->preserve_original_size);
    lua_pushnumber(L, ent->renderer->type);

    return 14;
}

int ye_lua_image_renderer_query(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not query renderer: entity is null\n");
        return 0;
    }

    lua_pushstring(L, ent->renderer->renderer_impl.image->src);

    return 1;
}

int ye_lua_renderer_modify(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not modify renderer: entity is null\n");
        return 0;
    }

    if(lua_isboolean(L, 2)){
        ent->renderer->active = lua_toboolean(L, 2);
    }

    if(lua_isboolean(L, 3)){
        ent->renderer->relative = lua_toboolean(L, 3);
    }

    if(lua_isnumber(L, 4)){
        ent->renderer->alpha = luaL_checknumber(L, 4);
    }

    if(lua_isnumber(L, 5)){
        ent->renderer->z = luaL_checknumber(L, 5);
    }

    if(lua_isnumber(L, 6)){
        ent->renderer->rect.x = luaL_checknumber(L, 6);
    }

    if(lua_isnumber(L, 7)){
        ent->renderer->rect.y = luaL_checknumber(L, 7);
    }

    if(lua_isnumber(L, 8)){
        ent->renderer->rect.w = luaL_checknumber(L, 8);
    }

    if(lua_isnumber(L, 9)){
        ent->renderer->rect.h = luaL_checknumber(L, 9);
    }

    if(lua_isnumber(L, 10)){
        ent->renderer->alignment = luaL_checknumber(L, 10);
    }

    if(lua_isnumber(L, 11)){
        ent->renderer->rotation = luaL_checknumber(L, 11);
    }

    if(lua_isboolean(L, 12)){
        ent->renderer->flipped_x = lua_toboolean(L, 12);
    }

    if(lua_isboolean(L, 13)){
        ent->renderer->flipped_y = lua_toboolean(L, 13);
    }

    if(lua_isboolean(L, 14)){
        ent->renderer->preserve_original_size = lua_toboolean(L, 14);
    }

    return 0;
}

int ye_lua_image_renderer_modify(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);
    
    if(ent == NULL) {
        ye_logf(error, "could not modify camera: entity is null\n");
        return 0;
    }

    // get the src
    if(lua_isstring(L, 2)){
        const char * path = luaL_checkstring(L, 2);
        free(ent->renderer->renderer_impl.image->src);
        ent->renderer->renderer_impl.image->src = malloc(strlen(path) + 1);
        strcpy(ent->renderer->renderer_impl.image->src, path);

        // reload the image
        ye_update_renderer_component(ent);
    }

    return 0;
}



void ye_lua_renderer_register(lua_State *L) {
    // init
    lua_register(L, "ye_lua_create_image_renderer", ye_lua_create_image_renderer);

    // query
    lua_register(L, "ye_lua_renderer_query", ye_lua_renderer_query);
    lua_register(L, "ye_lua_image_renderer_query", ye_lua_image_renderer_query);

    // modify
    lua_register(L, "ye_lua_renderer_modify", ye_lua_renderer_modify);
    lua_register(L, "ye_lua_image_renderer_modify", ye_lua_image_renderer_modify);
}