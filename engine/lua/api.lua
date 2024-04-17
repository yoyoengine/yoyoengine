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

-- NOTE: this is the lower level bridge api, that is abstracted by methods in Entity class

function ye_lua_ent_set_active(entity, state) end

function ye_lua_ent_get_entity_named(name) end



-- ---@class Entity
-- Entity = {}

-- --- Setup a new entity
-- function Entity:new() end

-- ----------------

-- --- Initialize the entity to an existing entity using name
-- --- @param name string The name of the entity
-- --- @return boolean True if the entity was found, false otherwise
-- function Entity:get_entity_by_name(name) end

-- --- Initialize the entity to an existing entity using id
-- --- @param id number The id of the entity
-- --- @return boolean True if the entity was found, false otherwise
-- function Entity:get_entity_by_id(id) end



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