--[[
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
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

---@param id integer The ID of the entity to get
---@return lightuserdata entity The pointer to the C entity
function ye_lua_ent_get_entity_by_id(id) end

---@param tag string The tag to search for
---@return lightuserdata entity The pointer to the C entity
function ye_lua_ent_get_entity_by_tag(tag) end

---@param name? string (optional) The name of the entity to create
---@return lightuserdata entity The pointer to the C entity
function ye_lua_create_entity(name) end

---@param entity lightuserdata The pointer to the C entity
function ye_lua_delete_entity(entity) end

---@param entity lightuserdata The pointer to the C entity to duplicate
---@return lightuserdata newEntity The pointer to the new entity created
function ye_lua_duplicate_entity(entity) end



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
---@param metaFile string The path to the animation meta file relative to resources/
---@param z number The z index of the renderer
function ye_lua_create_animation_renderer(entity,metaFile,z) end

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

---@param entity lightuserdata The pointer to the C entity
---@return boolean paused
---@return string metaFile
---@return number frameDelay
---@return number currentFrame
---@return number frameCount
---@return number frameWidth 
---@return number frameHeight
---@return string imageHandle
function ye_lua_animation_renderer_query(entity) end

---@param entity lightuserdata The pointer to the C entity
---@param paused boolean | nil
---@param metaFile string | nil
---@param frameDelay number | nil
---@param currentFrame number | nil
---@param frameCount number | nil
---@param frameWidth number | nil
---@param frameHeight number | nil
---@param imageHandle string | nil
function ye_lua_animation_renderer_modify(entity,paused,metaFile,frameDelay,currentFrame,frameCount,frameWidth,frameHeight,imageHandle) end

---**Forces the renderer to update and reconstruct itself**
---
---@param entity lightuserdata The pointer to the C entity
function ye_lua_force_refresh_renderer(entity) end



-------------------
--  Button API   --
-------------------

---@param entity lightuserdata The pointer to the C entity
---@param x number The x position to create the button at
---@param y number The y position to create the button at
---@param w number The width of the button
---@param h number The height of the button
function ye_lua_create_button(entity,x,y,w,h) end

---@param entity lightuserdata The pointer to the C entity
---@return boolean  isActive The active state
---@return boolean  isRelative The relative state
---@return number   x The x position of the button
---@return number   y The y position of the button
---@return number   w The width of the button
---@return number   h The height of the button
---@return boolean  isHovered Whether the button is hovered or not
---@return boolean  isPressed Whether the button is pressed down
---@return boolean  isClicked Whether the button was released after a press for a "click"
function ye_lua_button_query(entity) end

---**Modify a button by handle**
---
---Any parameters passed as nil will be left untouched
---
---@param entity        lightuserdata   The pointer to the C entity
---@param isActive      boolean | nil   The desired active state
---@param isRelative    boolean | nil   The desired relative state
---@param x             number | nil    The desired x position
---@param y             number | nil    The desired y position
---@param w             number | nil    The desired width
---@param h             number | nil    The desired height
function ye_lua_button_modify(entity,isActive,isRelative,x,y,w,h) end

---**Check the state of the button component on an entity**
---
---@param entity lightuserdata The pointer to the C entity
---@param queryType number The type of query to perform (1 = isHovered, 2 = isPressed, 3 = isClicked)
---@return boolean state The state of the button (true = yes, false = no)
function ye_lua_button_check_state(entity, queryType) end



-------------------
--    Tag API    --
-------------------

---Creates a new tag component
---@param entity lightuserdata The pointer to the C entity
function ye_lua_create_tag(entity) end

---**Query a tag component on an entity**
---
---@param entity lightuserdata The pointer to the C entity
---@return boolean isActive The active state
function ye_lua_tag_query(entity) end

---**Modify a tag component on an entity**
---
---@param entity lightuserdata The pointer to the C entity
---@param isActive boolean | nil The desired active state
function ye_lua_tag_modify(entity, isActive) end

---**Check if a tag component has a specific tag**
---
---@param entity lightuserdata The pointer to the C entity
---@param tag string The tag to check for
---@return boolean hasTag Whether the tag exists on the entity or not
function ye_lua_tag_has_tag(entity, tag) end

---**Add a tag to a tag component**
---
---@param entity lightuserdata The pointer to the C entity
---@param tag string The tag to add
function ye_lua_tag_add_tag(entity, tag) end

---**Remove a tag from a tag component**
---
---@param entity lightuserdata The pointer to the C entity
---@param tag string The tag to remove
function ye_lua_tag_remove_tag(entity, tag) end



------------------
-- Collider API --
------------------

---**Create a new static collider component.**
---
---@param entity lightuserdata The pointer to the C entity
---@param x number The x position of the collider
---@param y number The y position of the collider
---@param w number The width of the collider
---@param h number The height of the collider
function ye_lua_create_static_collider(entity, x, y, w, h) end

---**Create a new trigger collider component.**
---
---@param entity lightuserdata The pointer to the C entity
---@param x number The x position of the collider
---@param y number The y position of the collider
---@param w number The width of the collider
---@param h number The height of the collider
function ye_lua_create_trigger_collider(entity, x, y, w, h) end

---**Query a collider component on an entity**
---
---@param entity lightuserdata The pointer to the C entity
---@return boolean isActive The active state
---@return boolean isRelative The relative state
---@return number x The x position of the collider
---@return number y The y position of the collider
---@return number w The width of the collider
---@return number h The height of the collider
---@return boolean isTrigger Whether the collider is a trigger
function ye_lua_collider_query(entity) end

---**Modify a collider component on an entity**
---
---@param entity lightuserdata The pointer to the C entity
---@param isActive boolean | nil The desired active state
---@param isRelative boolean | nil The desired relative state
---@param x number | nil The desired x position of the collider
---@param y number | nil The desired y position of the collider
---@param w number | nil The desired width of the collider
---@param h number | nil The desired height of the collider
---@param isTrigger boolean | nil The desired trigger state
function ye_lua_collider_modify(entity, isActive, isRelative, x, y, w, h, isTrigger) end



------------------
-- Physics API  --
------------------

---**Create a new physics component.**
---
---@param entity lightuserdata The pointer to the C entity
---@param xVelocity number The x velocity (in pixels per second)
---@param yVelocity number The y velocity (in pixels per second)
function ye_lua_create_physics_component(entity, xVelocity, yVelocity) end

---**Query a physics component on an entity**
---
---@param entity lightuserdata The pointer to the C entity
---@return boolean isActive The active state
---@return number xVelocity The x velocity (in pixels per second)
---@return number yVelocity The y velocity (in pixels per second)
---@return number rotationalVelocity The rotational velocity (in degrees per second)
function ye_lua_physics_query(entity) end

---**Modify a physics component on an entity**
---
---@param entity lightuserdata The pointer to the C entity
---@param isActive boolean | nil The desired active state
---@param xVelocity number | nil The desired x velocity (in pixels per second)
---@param yVelocity number | nil The desired y velocity (in pixels per second)
---@param rotationalVelocity number | nil The desired rotational velocity (in degrees per second)
function ye_lua_physics_modify(entity, isActive, xVelocity, yVelocity, rotationalVelocity) end



-------------------
-- LuaScript API --
-------------------

---**Create a new LuaScript component.**
---
---@param entity lightuserdata The pointer to the C entity
---@param handle string The resource handle of the script to load
function ye_lua_create_lua_script(entity, handle) end

---**Modify a LuaScript component**
---
---@param entity lightuserdata The pointer to the C entity
---@return boolean isActive The active state
---@return string scriptHandle The resource handle the script was loaded from
function ye_lua_lua_script_modify(entity, isActive, scriptHandle) end

---**Query a LuaScript component**
---
---@param entity lightuserdata The pointer to the C entity
---@return boolean isActive The active state
---@return string scriptHandle The resource handle the script was loaded from
function ye_lua_lua_script_query(entity) end



----------------
-- Scene API  --
----------------

---**Load a scene by handle**
---
---@param handle string The path to the scene to load relative to resources/
function ye_load_scene(handle) end



----------------
-- Audio API  --
----------------

---**Plays a sound effect**
---
---@param handle string The resource handle
---@param loops number The number of times to loop the sound
---@param volume_scale number The volume scale
function ye_audio_play_sound(handle, loops, volume_scale) end

---**Plays a music track**
---
---@param handle string The resource handle
---@param loops number The number of times to loop the music
---@param volume_scale number The volume scale
function ye_audio_play_music(handle, loops, volume_scale) end

---**Set the volume of the audio system**
---
---@param volume_scale number The volume scale
function ye_audio_set_volume(volume_scale) end



----------------
-- Timer API  --
----------------

---**Creates a new engine timer**
---
---@param duration_ms number The duration of the timer in milliseconds
---@param callback_fn function The function to call when the timer expires
---@param loops number The number of times to loop the timer. 0 means loop forever
---@param start_ticks number The number of ticks to wait before starting the timer
---@vararg any Additional arguments to pass to the callback function when resolved
function ye_lua_timer_create_timer(duration_ms, callback_fn, loops, start_ticks, ...) end

---**Gets the engine SDL_ticks**
---
---@return number ticks The current ticks of the engine
function ye_lua_timer_get_ticks() end

----------------
-- Input API  --
----------------

---**Retrieves the state of the mouse**
---
---@return any mouse_state The state of the mouse (table)
function ye_lua_input_query_mouse() end

---**Retrieves the state of a key**
---
---@param isKeycode boolean Whether the key is a keycode or not (false = scancode)
---@param code number The code of the key to query
---@return boolean state The state of the key
function ye_lua_input_query_key(isKeycode, code) end

---**Retrieves the state of a mod**
---
---@param mod number The mod to query
---@return boolean state The state of the mod
function ye_lua_input_query_mod(mod) end

---**Retrieves the state of a controller**
---
---@param controllerNum number The number of the controller to query
---@return any controller_state The state of the controller (table)
function ye_lua_input_query_controller(controllerNum) end

---**Retrieves the number of controllers connected**
---
---@return integer numControllers The number of controllers connected
function ye_lua_input_number_of_controllers() end



----------------
--    misc    --
----------------

---**Quit the game**
---
---@return nil
function exitGame() end

---**Check if a component exists on an entity**
---
---@param entity lightuserdata The pointer to the C entity
---@param comp_indx number The index of the component to check for
--- 0: Transform
--- 1: Renderer
--- 2: Camera
--- 3: Script
--- 4: Button
--- 5: Physics
--- 6: Collider
--- 7: Tag
--- 8: Audiosource
---@return boolean exists Whether the component exists or not
function ye_lua_check_component_exists(entity, comp_indx) end

---**Check if a renderer component type exists on an entity**
---
---@param entity lightuserdata The pointer to the C entity
---@param type_indx number The index of the renderer component type to check for
--- 0: text
--- 1: text outlined
--- 2: image
--- 3: animation
--- 4: tile
---@return boolean exists Whether the renderer component type exists or not
function ye_lua_check_renderer_component_type_exists(entity, type_indx) end

---**Invoke a function on another state
---
---@param entity lightuserdata The pointer to the C entity
---@param sig string The signature of the function to call
---@vararg any Arguments to pass to the function
---@return any The return value(s) of the function
function ye_invoke_cross_state_function(entity, sig, ...) end

---**Read a global value from another state**
---
---@param entity lightuserdata The pointer to the C entity
---@param key string The key of the global value to read
---@return any The value of the global
function ye_read_cross_state_value(entity, key) end

---**Write a global value to another state**
---
---@param entity lightuserdata The pointer to the C entity
---@param key string The key of the global value to write
---@param value any The value to write to the global
function ye_write_cross_state_value(entity, key, value) end

---**Removes a component from an entity**
---
---@param entity lightuserdata The pointer to the C entity
---@param comp_indx number The index of the component to remove
--- 0: Transform
--- 1: Renderer
--- 2: Camera
--- 3: Script
--- 4: Button
--- 5: Physics
--- 6: Collider
--- 7: Tag
--- 8: Audiosource
function ye_lua_remove_component(entity, comp_indx) end