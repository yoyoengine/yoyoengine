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