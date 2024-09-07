/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <string.h>

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
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not create renderer: entity is null\n");
        return 0;
    }

    // get path, z
    const char * path = luaL_checkstring(L, 2);
    int z = luaL_checknumber(L, 3);

    ye_add_image_renderer_component(ent, z, path);

    return 0;
}



int ye_lua_create_text_renderer(lua_State *L) {
    ye_logf(debug, "creating text renderer\n");
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not create renderer: entity is null\n");
        return 0;
    }

    // text, font_name, font size, color name (wrap with can be set afterwards or: TODO: overload later)
    const char * text = luaL_checkstring(L, 2);
    const char * font_name = luaL_checkstring(L, 3);
    int font_size = luaL_checknumber(L, 4);
    const char * color_name = luaL_checkstring(L, 5);
    int z = luaL_checknumber(L, 6);

    ye_logf(debug, "adding text renderer comp\n");
    ye_add_text_renderer_component(ent, z, text, font_name, font_size, color_name, /*no wrap*/0);

    return 0;
}



int ye_lua_text_renderer_query(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not query renderer: entity is null\n");
        return 0;
    }

    lua_pushstring(L, ent->renderer->renderer_impl.text->text);
    lua_pushstring(L, ent->renderer->renderer_impl.text->font_name);
    lua_pushnumber(L, ent->renderer->renderer_impl.text->font_size);
    lua_pushstring(L, ent->renderer->renderer_impl.text->color_name);
    lua_pushnumber(L, ent->renderer->renderer_impl.text->wrap_width);

    return 5;
}



int ye_lua_text_renderer_modify(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not modify renderer: entity is null\n");
        return 0;
    }

    // text
    if(lua_isstring(L, 2)){
        const char * text = luaL_checkstring(L, 2);
        free(ent->renderer->renderer_impl.text->text);
        ent->renderer->renderer_impl.text->text = malloc(strlen(text) + 1);
        strcpy(ent->renderer->renderer_impl.text->text, text);
    }

    // font name
    if(lua_isstring(L, 3)){
        const char * font_name = luaL_checkstring(L, 3);
        free(ent->renderer->renderer_impl.text->font_name);
        ent->renderer->renderer_impl.text->font_name = malloc(strlen(font_name) + 1);
        strcpy(ent->renderer->renderer_impl.text->font_name, font_name);
    }

    // font size
    if(lua_isnumber(L, 4)){
        ent->renderer->renderer_impl.text->font_size = luaL_checknumber(L, 4);
    }

    // color name
    if(lua_isstring(L, 5)){
        const char * color_name = luaL_checkstring(L, 5);
        free(ent->renderer->renderer_impl.text->color_name);
        ent->renderer->renderer_impl.text->color_name = malloc(strlen(color_name) + 1);
        strcpy(ent->renderer->renderer_impl.text->color_name, color_name);
    }

    // wrap width
    if(lua_isnumber(L, 6)){
        ent->renderer->renderer_impl.text->wrap_width = luaL_checknumber(L, 6);
    }

    // reflect any changes made
    ye_update_renderer_component(ent);

    return 0;
}



int ye_lua_create_text_outlined_renderer(lua_State *L) {
    ye_logf(debug, "creating text renderer\n");
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not create renderer: entity is null\n");
        return 0;
    }

    // text, font_name, font size, color name, outlineSize, outlineColorName (wrap with can be set afterwards or: TODO: overload later)
    const char * text = luaL_checkstring(L, 2);
    const char * font_name = luaL_checkstring(L, 3);
    int font_size = luaL_checknumber(L, 4);
    const char * color_name = luaL_checkstring(L, 5);
    int outline_size = luaL_checknumber(L, 6);
    const char * outline_color_name = luaL_checkstring(L, 7);
    int z = luaL_checknumber(L, 8);

    ye_logf(debug, "adding text renderer comp\n");
    ye_add_text_outlined_renderer_component(ent, z, text, font_name, font_size, color_name, outline_color_name, outline_size, /*no wrap*/0);

    return 0;
}



int ye_lua_create_tile_renderer(lua_State *L) {
    ye_logf(debug, "creating tile renderer\n");
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not create renderer: entity is null\n");
        return 0;
    }

    // tileset, tile, z, srcX, srcY, srcW, srcH
    const char * tileset = luaL_checkstring(L, 2);
    int srcX = luaL_checknumber(L, 3);
    int srcY = luaL_checknumber(L, 4);
    int srcW = luaL_checknumber(L, 5);
    int srcH = luaL_checknumber(L, 6);
    int z = luaL_checknumber(L, 7);

    ye_logf(debug, "adding tile renderer comp\n");
    ye_add_tilemap_renderer_component(ent, z, tileset, (SDL_Rect){srcX, srcY, srcW, srcH});

    return 0;
}



int ye_lua_tile_renderer_query(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not query renderer: entity is null\n");
        return 0;
    }

    lua_pushstring(L, ent->renderer->renderer_impl.tile->handle);
    lua_pushnumber(L, ent->renderer->renderer_impl.tile->src.x);
    lua_pushnumber(L, ent->renderer->renderer_impl.tile->src.y);
    lua_pushnumber(L, ent->renderer->renderer_impl.tile->src.w);
    lua_pushnumber(L, ent->renderer->renderer_impl.tile->src.h);

    return 5;
}



int ye_lua_tile_renderer_modify(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not modify renderer: entity is null\n");
        return 0;
    }

    // tileset
    if(lua_isstring(L, 2)){
        const char * tileset = luaL_checkstring(L, 2);
        free(ent->renderer->renderer_impl.tile->handle);
        ent->renderer->renderer_impl.tile->handle = malloc(strlen(tileset) + 1);
        strcpy(ent->renderer->renderer_impl.tile->handle, tileset);
    }

    // srcX
    if(lua_isnumber(L, 3)){
        ent->renderer->renderer_impl.tile->src.x = luaL_checknumber(L, 3);
    }

    // srcY
    if(lua_isnumber(L, 4)){
        ent->renderer->renderer_impl.tile->src.y = luaL_checknumber(L, 4);
    }

    // srcW
    if(lua_isnumber(L, 5)){
        ent->renderer->renderer_impl.tile->src.w = luaL_checknumber(L, 5);
    }

    // srcH
    if(lua_isnumber(L, 6)){
        ent->renderer->renderer_impl.tile->src.h = luaL_checknumber(L, 6);
    }

    // reflect any changes made
    ye_update_renderer_component(ent);

    return 0;
}



int ye_lua_create_animation_renderer(lua_State *L) {
    ye_logf(debug, "creating animation renderer\n");
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not create renderer: entity is null\n");
        return 0;
    }

    // animation, z
    const char * animation = luaL_checkstring(L, 2);
    int z = luaL_checknumber(L, 3);

    ye_logf(debug, "adding animation renderer comp\n");
    ye_add_animation_renderer_component(ent, z, animation);

    return 0;
}



int ye_lua_animation_renderer_query(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not query renderer: entity is null\n");
        return 0;
    }

    lua_pushboolean(L, ent->renderer->renderer_impl.animation->paused);
    lua_pushstring(L, ent->renderer->renderer_impl.animation->meta_file);
    lua_pushnumber(L, ent->renderer->renderer_impl.animation->frame_delay);
    lua_pushnumber(L, ent->renderer->renderer_impl.animation->current_frame_index);
    lua_pushnumber(L, ent->renderer->renderer_impl.animation->frame_count);
    lua_pushnumber(L, ent->renderer->renderer_impl.animation->frame_width);
    lua_pushnumber(L, ent->renderer->renderer_impl.animation->frame_height);
    lua_pushstring(L, ent->renderer->renderer_impl.animation->animation_handle);

    return 8;
}



int ye_lua_animation_renderer_modify(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not modify renderer: entity is null\n");
        return 0;
    }

    // paused
    if(lua_isboolean(L, 2)){
        ent->renderer->renderer_impl.animation->paused = lua_toboolean(L, 2);
    }

    // meta file
    if(lua_isstring(L, 3)){
        const char * meta_file = luaL_checkstring(L, 3);
        free(ent->renderer->renderer_impl.animation->meta_file);
        ent->renderer->renderer_impl.animation->meta_file = malloc(strlen(meta_file) + 1);
        strcpy(ent->renderer->renderer_impl.animation->meta_file, meta_file);
    }

    // frame delay
    if(lua_isnumber(L, 4)){
        ent->renderer->renderer_impl.animation->frame_delay = luaL_checknumber(L, 4);
    }

    // current frame index
    if(lua_isnumber(L, 5)){
        ent->renderer->renderer_impl.animation->current_frame_index = luaL_checknumber(L, 5);
    }

    // frame count
    if(lua_isnumber(L, 6)){
        ent->renderer->renderer_impl.animation->frame_count = luaL_checknumber(L, 6);
    }

    // frame width
    if(lua_isnumber(L, 7)){
        ent->renderer->renderer_impl.animation->frame_width = luaL_checknumber(L, 7);
    }

    // frame height
    if(lua_isnumber(L, 8)){
        ent->renderer->renderer_impl.animation->frame_height = luaL_checknumber(L, 8);
    }

    // animation handle
    if(lua_isstring(L, 9)){
        const char * animation_handle = luaL_checkstring(L, 9);
        free(ent->renderer->renderer_impl.animation->animation_handle);
        ent->renderer->renderer_impl.animation->animation_handle = malloc(strlen(animation_handle) + 1);
        strcpy(ent->renderer->renderer_impl.animation->animation_handle, animation_handle);
    }

    // reflect any changes made
    ye_update_renderer_component(ent);

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

    // ye_logf(debug, "renderer active is a %s\n", luaL_typename(L, 2));
    // ye_logf(debug, "renderer relative is a %s\n", luaL_typename(L, 3));
    // ye_logf(debug, "renderer alpha is a %s\n", luaL_typename(L, 4));
    // ye_logf(debug, "renderer z is a %s\n", luaL_typename(L, 5));
    // ye_logf(debug, "renderer x is a %s\n", luaL_typename(L, 6));
    // ye_logf(debug, "renderer y is a %s\n", luaL_typename(L, 7));
    // ye_logf(debug, "renderer w is a %s\n", luaL_typename(L, 8));
    // ye_logf(debug, "renderer h is a %s\n", luaL_typename(L, 9));
    // ye_logf(debug, "renderer alignment is a %s\n", luaL_typename(L, 10));
    // ye_logf(debug, "renderer rotation is a %s\n", luaL_typename(L, 11));
    // ye_logf(debug, "renderer flipX is a %s\n", luaL_typename(L, 12));
    // ye_logf(debug, "renderer flipY is a %s\n", luaL_typename(L, 13));
    // ye_logf(debug, "renderer preserveOriginalSize is a %s\n", luaL_typename(L, 14));

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

    // ye_update_renderer_component(ent);

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



int ye_lua_force_refresh_renderer(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not force refresh renderer: entity is null\n");
        return 0;
    }

    ye_update_renderer_component(ent);

    return 0;
}



void ye_lua_renderer_register(lua_State *L) {
    // renderer
    lua_register(L, "ye_lua_renderer_query", ye_lua_renderer_query);
    lua_register(L, "ye_lua_renderer_modify", ye_lua_renderer_modify);

    // image
    lua_register(L, "ye_lua_create_image_renderer", ye_lua_create_image_renderer);
    lua_register(L, "ye_lua_image_renderer_query", ye_lua_image_renderer_query);
    lua_register(L, "ye_lua_image_renderer_modify", ye_lua_image_renderer_modify);

    // text
    lua_register(L, "ye_lua_create_text_renderer", ye_lua_create_text_renderer);
    lua_register(L, "ye_lua_text_renderer_query", ye_lua_text_renderer_query);
    lua_register(L, "ye_lua_text_renderer_modify", ye_lua_text_renderer_modify);

    // text outlined
    lua_register(L, "ye_lua_create_text_outlined_renderer", ye_lua_create_text_outlined_renderer);
    lua_register(L, "ye_lua_text_outlined_renderer_query", ye_lua_text_renderer_query);
    lua_register(L, "ye_lua_text_outlined_renderer_modify", ye_lua_text_renderer_modify);

    // tile
    lua_register(L, "ye_lua_create_tile_renderer", ye_lua_create_tile_renderer);
    lua_register(L, "ye_lua_tile_renderer_query", ye_lua_tile_renderer_query);
    lua_register(L, "ye_lua_tile_renderer_modify", ye_lua_tile_renderer_modify);

    // animation
    lua_register(L, "ye_lua_create_animation_renderer", ye_lua_create_animation_renderer);
    lua_register(L, "ye_lua_animation_renderer_query", ye_lua_animation_renderer_query);
    lua_register(L, "ye_lua_animation_renderer_modify", ye_lua_animation_renderer_modify);

    // misc
    lua_register(L, "ye_lua_force_refresh_renderer", ye_lua_force_refresh_renderer);
}