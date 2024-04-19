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