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

--[[
    +--------------------------------------------------------------------------------+
    | This file is a critical component of the yoyoengine lua runtime.               |
    | It creates an object oriented interface to the data oriented yoyoengine C API. |
    | This file will be bundled into engine.yep upon all game builds.                |
    +--------------------------------------------------------------------------------+
    | yoyoengine lua runtime version 0 - Ryan Zmuda, 2024                            |
    +--------------------------------------------------------------------------------+
--]]

-- define NULL to be a void pointer to nil (used to check for NULL C pointers)
-- local NULL = ffi.new("void *", nil)

-- define the Entity class
Entity = {
    -- reference to the C pointer
    _c_entity = nil,

    -- references to the components
    Transform = nil, 

    setActive = function(self, state)
        -- log("info", "Entity:setActive called\n")

        -- log the self._c_entity pointer
        -- log("info", "self._c_entity is " .. tostring(self._c_entity) .. "\n")

        -- check if entity is nil
        if self._c_entity == nil then
            log("error", "Entity:setActive called on nil entity\n")
            return
        end

        -- log("info", "cleared nil check\n")

        -- check if state is not boolean
        if type(state) ~= "boolean" then
            log("error", "Entity:setActive called with non-boolean state\n")
            return
        end

        -- log("info", "cleared bool check\n")

        ye_lua_ent_set_active(self._c_entity, state)
    end,
}

-- define the entity metatable
Entity_mt = {
    __index = Entity
}

-- define the Transform class
Transform = {
    -- reference to the C pointer
    _c_component = nil,

    setPosition = function(self, x, y)
        log("info", "Transform:setPosition called\n")
        -- TODO : call the C API to set the position
    end,
}

-- define the transform metatable
Transform_mt = {
    __index = Transform
}

---**Create a new entity.**
---
---@param name string The name of the entity to create (optional)
---@return table entity The lua entity object
---If this function fails, you will get errors as well as an entity
---object with a nil _c_entity pointer.
---
---example:
---```lua
---local prop_box = Entity:newEntity("BOX")
---```
function Entity:new(name)
    -- name is optional TODO: clean thhis up and add behavior

    -- create the entity itself
    local entity = {}
    setmetatable(entity, Entity_mt)

    -- create the transform component
    entity.Transform = {}
    setmetatable(entity.Transform, Transform_mt)

    -- get the _c_entity pointer
    
    
    return entity
end

---**Get a scene entity by name.**
---
---@param name string The name of the entity to get
---@return table entity The lua entity object
---If this function fails, you will get errors as well as an entity
---object with a nil _c_entity pointer.
---
---example:
---```lua
---local player = Entity:getEntityNamed("PLAYER")
---```
function Entity:getEntityNamed(name)
    -- create the entity itself
    local entity = {}
    setmetatable(entity, Entity_mt)

    -- get the _c_entity pointer
    entity._c_entity = ye_lua_ent_get_entity_named(name)
    if entity._c_entity == nil then
        log("error", "Entity:getEntityNamed failed to find entity\n")
        -- for now lets keep initializing because its better to create meta for
        -- a broken entity, than to return a nil entity, since the script has a
        -- better chance of recovery
    end

    -- create the transform component
    entity.Transform = {}
    setmetatable(entity.Transform, Transform_mt)

    return entity
end

-- Make Entity and Transform global
_G["Entity"] = Entity
_G["Transform"] = Transform

log("debug", "bootstrapped runtime.lua onto new VM\n")