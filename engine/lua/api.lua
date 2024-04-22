--[[
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
]]

---@meta

---**called once when the script is mounted in engine**
function onMount() end

---**called once per frame (before render pass) when the script is active**
function onUpdate() end

---**called once when the script is unmounted from engine**
function onUnmount() end

---**Log a message through the engine's logging system.**
---
---@param level string The log level
---@param message string The message to log
---@return nil
---make sure level is one of the following:
---"debug", "info", "warning", "error"
---and that you include a newline at the end if desired
---
---example:
---```lua
---ye_log("info","hello world!\n")
---```
function log(level, message) end


----------------
-- Entity API --
----------------

---@param entity lightuserdata The pointer to the C entity
---@return boolean state The active state
function ye_lua_ent_get_active(entity) end

---@param entity lightuserdata The pointer to the C entity
---@param state boolean The desired active state
function ye_lua_ent_set_active(entity, state) end

---@param entity lightuserdata The pointer to the C entity
---@return integer ID The ECS ID number of the entity (-1 for failure)
function ye_lua_ent_get_id(entity) end

---@param entity lightuserdata The pointer to the C entity
---@return string name The name of the entity
function ye_lua_ent_get_name(entity) end

---@param entity lightuserdata The pointer to the C entity
---@param name string The desired name of the entity
function ye_lua_ent_set_name(entity, name) end

---@param name string The name of the entity
---@return lightuserdata entity The pointer to the C entity
function ye_lua_ent_get_entity_named(name) end

---@param name? string (optional) The name of the entity to create
---@return lightuserdata entity The pointer to the C entity
function ye_lua_create_entity(name) end



-------------------
-- Transform API --
-------------------

---@param entity lightuserdata The pointer to the C entity
---@param x number The x position to create the transform at
---@param y number The y position to create the transform at
function ye_lua_create_transform(entity,x,y) end

---@param entity lightuserdata The pointer to the C entity
---@return number x The x position of the transform
function ye_lua_transform_get_position_x(entity) end

---@param entity lightuserdata The pointer to the C entity
---@return number y The y position of the transform
function ye_lua_transform_get_position_y(entity) end

---@param entity lightuserdata The pointer to the C entity
---@param x number The new x position of the transform
function ye_lua_transform_set_position_x(entity,x) end

---@param entity lightuserdata The pointer to the C entity
---@param y number The new y position of the transform
function ye_lua_transform_set_position_y(entity,y) end



-------------------
--  Camera API   --
-------------------

---@param entity lightuserdata The pointer to the C entity
---@param x number The x position to create the camera at
---@param y number The y position to create the camera at
---@param w number The width of the camera
---@param h number The height of the camera
---@param z number The z index of the camera
function ye_lua_create_camera(entity,x,y,w,h,z) end

---@param entity lightuserdata The pointer to the C entity
---@return boolean  isActive The active state
---@return boolean  isRelative The relative state
---@return number   z The z index of the camera
---@return number   x The x position of the camera
---@return number   y The y position of the camera
---@return number   w The width of the camera
---@return number   h The height of the camera
function ye_lua_camera_query(entity) end

---**Modify a camera by handle**
---
---Any parameters passed as nil will be left untouched
---
---@param entity        lightuserdata   The pointer to the C entity
---@param isActive      boolean | nil   The desired active state
---@param isRelative    boolean | nil   The desired relative state
---@param z             number | nil    The desired z index
---@param x             number | nil    The desired x position
---@param y             number | nil    The desired y position
---@param w             number | nil    The desired width
---@param h             number | nil    The desired height
function ye_lua_camera_modify(entity,isActive,isRelative,z,x,y,w,h) end



-------------------
-- Renderer API  --
-------------------

---@param entity lightuserdata The pointer to the C entity
---@param handle string The path to the image to render relative to resources/
---@param z number The z index of the renderer
function ye_lua_create_image_renderer(entity,handle,z) end

---@param entity lightuserdata The pointer to the C entity
---@param text string The text to render
---@param font_name string The name of the (cached) font to use
---@param font_size number The size of the font to use
---@param color_name string The name of the (cached) color to use
---@param z number The z index of the renderer
function ye_lua_create_text_renderer(entity, text, font_name, font_size, color_name, z) end

---@param entity lightuserdata The pointer to the C entity
---@param text string The text to render
---@param fontName string The name of the (cached) font to use
---@param fontSize number The size of the font to use
---@param colorName string The name of the (cached) color to use
---@param outlineSize number The size of the outline to render (pixels i think?)
---@param outlineColorName string The name of the (cached) color to use for the outline
---@param z number The z index of the renderer
function ye_lua_create_text_outlined_renderer(entity, text, fontName, fontSize, colorName, outlineSize, outlineColorName, z) end

---@param entity lightuserdata The pointer to the C entity
---@param handle string The path to the source image relative to resources/
---@param srcX number The x position of the tile to render
---@param srcY number The y position of the tile to render
---@param srcW number The width of the tile to render
---@param srcH number The height of the tile to render
---@param z number The z index of the renderer
function ye_lua_create_tile_renderer(entity,handle,srcX,srcY,srcW,srcH,z) end

---@param entity lightuserdata The pointer to the C entity
---@return boolean  isActive The active state
---@return boolean  isRelative The relative state
---@return number   alpha The alpha value of the renderer
---@return number   z The z index of the renderer
---@return number   x The x position of the renderer
---@return number   y The y position of the renderer
---@return number   w The width of the renderer
---@return number   h The height of the renderer
---@return number   alignment The alignment of the renderer
---@return number   rotation The clockwise rotation in degrees to render at
---@return boolean  flipX Flip the renderer on the x axis
---@return boolean  flipY Flip the renderer on the y axis
---@return boolean  preserveOriginalSize Controls whether the texture grows/shrinks or stays the same size when aligining in bounds
---@return RendererType type The current type of the renderer_impl
function ye_lua_renderer_query(entity) end

---@param entity lightuserdata | nil The pointer to the C entity
---@param isActive boolean | nil The desired active state
---@param isRelative boolean | nil The desired relative state
---@param alpha number | nil The desired alpha value
---@param z number | nil The desired z index
---@param x number | nil The desired x position
---@param y number | nil The desired y position
---@param w number | nil The desired width
---@param h number | nil The desired height
---@param alignment number | nil The desired alignment
---@param rotation number | nil The desired clockwise rotation in degrees
---@param flipX boolean | nil The desired flip on the x axis
---@param flipY boolean | nil The desired flip on the y axis
---@param preserveOriginalSize boolean | nil The desired preserveOriginalSize state
function ye_lua_renderer_modify(entity,isActive,isRelative,alpha,z,x,y,w,h,alignment,rotation,flipX,flipY,preserveOriginalSize) end

---@param entity lightuserdata The pointer to the C entity
---@return string src
function ye_lua_image_renderer_query(entity) end

---@param entity lightuserdata The pointer to the C entity
---@param src string The path to the image to render relative to resources/
function ye_lua_image_renderer_modify(entity,src) end

---@param entity lightuserdata The pointer to the C entity
---@return string text
---@return string fontName
---@return number fontSize
---@return string colorName
---@return number wrapWidth
function ye_lua_text_renderer_query(entity) end

---@param entity lightuserdata The pointer to the C entity
---@param text string | nil The text to render
---@param fontName string | nil The name of the (cached) font to use
---@param fontSize number | nil The size of the font to use
---@param colorName string | nil The name of the (cached) color to use
---@param wrapWidth number | nil The width to wrap the text at
function ye_lua_text_renderer_modify(entity,text,fontName,fontSize,colorName,wrapWidth) end

---@param entity lightuserdata The pointer to the C entity
---@return string text
---@return string fontName
---@return number fontSize
---@return string colorName
---@return number wrapWidth
---@return number outlineSize
---@return string outlineColorName
function ye_lua_text_outlined_renderer_query(entity) end

---@param entity lightuserdata The pointer to the C entity
---@param text string | nil The text to render
---@param fontName string | nil The name of the (cached) font to use
---@param fontSize number | nil The size of the font to use
---@param colorName string | nil The name of the (cached) color to use
---@param wrapWidth number | nil The width to wrap the text at
---@param outlineSize number | nil The size of the outline to render (pixels i think?)
---@param outlineColorName string | nil The name of the (cached) color to use for the outline
function ye_lua_text_outlined_renderer_modify(entity,text,fontName,fontSize,colorName,wrapWidth,outlineSize,outlineColorName) end

---@param entity lightuserdata The pointer to the C entity
---@return string handle
---@return number srcX
---@return number srcY
---@return number srcW
---@return number srcH
function ye_lua_tile_renderer_query(entity) end

---@param entity lightuserdata The pointer to the C entity
---@param handle string | nil The path to the source image relative to resources/
---@param srcX number | nil The x position of the tile to render
---@param srcY number | nil The y position of the tile to render
---@param srcW number | nil The width of the tile to render
---@param srcH number | nil The height of the tile to render
function ye_lua_tile_renderer_modify(entity,handle,srcX,srcY,srcW,srcH) end



----------------
-- Scene API  --
----------------

---**Load a scene by handle**
---
---@param handle string The path to the scene to load relative to resources/
---@return nil
---example:
---```lua
---ye_load_scene("scenes/game.yoyo")
---```
function loadScene(handle) end



----------------
--    misc    --
----------------

---**Quit the game**
---
---@return nil
function exitGame() end