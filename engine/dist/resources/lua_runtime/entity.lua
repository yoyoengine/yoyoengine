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

---@class Entity
---@field _c_entity lightuserdata
---@field Transform Transform
---@field Camera Camera
---@field Renderer Renderer
Entity = {
    --- reference to the C pointer
    ---@type lightuserdata
    _c_entity = nil,
    
    -- references to the components
    
    ---@type Transform
    Transform = nil, 

    ---@type Camera
    Camera = nil,

    ---@type Renderer
    Renderer = nil,

    ---**Get the active state of the entity.**
    ---
    ---@return boolean state The active state of the entity
    ---
    ---example:
    ---```lua
    ---local prop_box = Entity:newEntity("BOX")
    ---print("box is active: ", prop_box:getActive())
    ---```
    getActive = function(self)
        if not ValidateEntity(self) then
            log("error", "Entity:getActive called for nil entity, returning false!\n")
            return false
        end

        return ye_lua_ent_get_active(self._c_entity)
    end,

    ---**Set the active state of the entity.**
    ---
    ---@param state boolean The desired active state
    ---
    ---example:
    ---```lua
    ---local prop_box = Entity:newEntity("BOX")
    ---prop_box:setActive(true)
    ---```
    setActive = function(self, state)
        -- log("info", "Entity:setActive called\n")

        if not ValidateEntity(self) then
            return
        end

        -- check if state is not boolean
        if type(state) ~= "boolean" then
            log("error", "Entity:setActive called with non-boolean state\n")
            return
        end

        ye_lua_ent_set_active(self._c_entity, state)
    end,

    ---**Get the ECS ID number of the entity.**
    ---
    ---@return integer ID The ECS ID number of the entity (-1 for failure)
    ---
    ---example:
    ---```lua
    ---local prop_box = Entity:newEntity("BOX")
    ---print("box ID: ", prop_box:getID())
    ---```
    getID = function(self)
        if not ValidateEntity(self) then
            return -1
        end

        return ye_lua_ent_get_id(self._c_entity)
    end,

    ---**Set the name of the entity.**
    ---
    ---@param name string The desired name of the entity
    ---
    ---example:
    ---```lua
    ---local prop_box = Entity:newEntity("BOX")
    ---prop_box:setName("BOX")
    ---```
    setName = function(self, name)
        if not ValidateEntity(self) then
            return
        end

        -- check if name is not string
        if type(name) ~= "string" then
            log("error", "Entity:setName called with non-string name\n")
            return
        end

        ye_lua_ent_set_name(self._c_entity, name)
    end,

    ---**Get the name of the entity.**
    ---
    ---@return string name The name of the entity
    ---
    ---example:
    ---```lua
    ---local prop_box = Entity:newEntity("BOX")
    ---print("box name: ", prop_box:getName())
    ---```
    getName = function(self)
        if not ValidateEntity(self) then
            return "ERROR"
        end

        return ye_lua_ent_get_name(self._c_entity)
    end,
}

-- define the entity metatable
Entity_mt = {
    __index = Entity
}

---**Create a new entity.**
---
---@param name? string The name of the entity to create (optional)
---@return Entity entity The lua entity object
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

    -- get the _c_entity pointer
    if name then
        entity._c_entity = ye_lua_create_entity(name)
    else
        entity._c_entity = ye_lua_create_entity()
    end

    return entity
end

---**Get a scene entity by name.**
---
---@param name string The name of the entity to get
---@return Entity entity The lua entity object
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

    -- TODO: YOU SHOULD AUTOMATICALLY GET COMPONENTS AND SUCH HERE

    return entity
end