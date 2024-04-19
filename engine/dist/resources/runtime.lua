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
YE_LUA_RUNTIME_VERSION = 0
YE_LUA_RUNTIME_AUTHOR  = "Ryan Zmuda"

--[[
    NOTES:
    - could mark some vars as private but then we cant access them across components...
]]

-------------- HELPER FUNCTIONS --------------

--- helper function that ensures _c_entity is not nil
---@param self table The entity object
---@return boolean True if the entity is valid, false otherwise
local function validateEntity(self)
    if self._c_entity == nil then
        log("error", "Entity method called for nil entity\n")
        return false
    end
    return true
end

----------------------------------------------



---------------- ENTITY TABLE ----------------

---@class Entity
---@field _c_entity lightuserdata
---@field Transform Transform
Entity = {
    --- reference to the C pointer
    ---@type lightuserdata
    _c_entity = nil,
    
    -- references to the components
    ---@type Transform
    Transform = nil, 





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
        if not validateEntity(self) then
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

        if not validateEntity(self) then
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
        if not validateEntity(self) then
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
        if not validateEntity(self) then
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
        if not validateEntity(self) then
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

    return entity
end

----------------------------------------------



-------------- TRANSFORM TABLE ---------------

---@class Transform
---@field parent Entity
---@field _c_component lightuserdata
Transform = {
    ---@type Entity
    parent = nil,

    ---@type lightuserdata
    _c_component = nil,





    ---**Set the x position of a transform.**
    ---
    ---@param x number The x position
    ---
    ---Will create a new transform component if one does not exist.
    ---
    ---example:
    ---```lua
    ---local player = Entity:getEntityNamed("PLAYER")
    ---player.Transform:setPosition(100, 200)
    ---```
    setPositionX = function(self, x)
        if not validateEntity(self.parent) then
            return
        end

        -- check if x is not a number
        if type(x) ~= "number" then
            log("error", "Transform:setPositionX called with non-number x\n")
            return
        end

        ye_lua_transform_set_position_x(self.parent._c_entity, x)
    end,





    ---**Set the y position of a transform.**
    ---
    ---@param y number The y position
    ---
    ---Will create a new transform component if one does not exist.
    ---
    ---example:
    ---```lua
    ---local player = Entity:getEntityNamed("PLAYER")
    ---player.Transform:setPosition(100, 200)
    ---```
    setPositionY = function(self, y)
        if not validateEntity(self.parent) then
            return
        end

        -- check if y is not a number
        if type(y) ~= "number" then
            log("error", "Transform:setPositionY called with non-number y\n")
            return
        end

        ye_lua_transform_set_position_y(self.parent._c_entity, y)
    end,




    
    ---**Set the position of a transform.**
    ---
    ---@param x number The x position
    ---@param y number The y position
    ---
    ---Will create a new transform component if one does not exist.
    ---
    ---example:
    ---```lua
    ---local player = Entity:getEntityNamed("PLAYER")
    ---player.Transform:setPosition(100, 200)
    ---```
    setPosition = function(self, x, y)
        -- omit sanity check, as both functions do it
        self.setPositionX(self, x)
        self.setPositionY(self, y)
    end,





    ---**Get the x position of the transform.**
    ---
    ---@return number x The x position
    ---
    ---Will return 0 if the transform component does not exist.
    ---
    ---example:
    ---```lua
    ---local player = Entity:getEntityNamed("PLAYER")
    ---local pos_x = player.Transform:getPositionX()
    ---```
    getPositionX = function(self)
        if not validateEntity(self.parent) then
            return 0
        end

        return ye_lua_transform_get_position_x(self.parent._c_entity)
    end,





    ---**Get the y position of the transform.**
    ---
    ---@return number y The y position
    ---
    ---Will return 0 if the transform component does not exist.
    ---
    ---example:
    ---```lua
    ---local player = Entity:getEntityNamed("PLAYER")
    ---local pos_y = player.Transform:getPositionY()
    ---```
    getPositionY = function(self)
        if not validateEntity(self.parent) then
            return 0
        end

        return ye_lua_transform_get_position_y(self.parent._c_entity)
    end,





    ---**Get the position of the transform.**
    ---
    ---@return number x The x position
    ---@return number y The y position
    ---
    ---Will return 0, 0 if the transform component does not exist.
    ---
    ---example:
    ---```lua
    ---local player = Entity:getEntityNamed("PLAYER")
    ---local pos_x, pos_y = player.Transform:getPosition()
    ---```
    getPosition = function(self)
        return self.getPositionX(self), self.getPositionY(self)
    end,
}



-- define the transform metatable
Transform_mt = {
    __index = Transform
}



---**Create a new transform component.**
---
---@param entity Entity The entity to attach the transform to
---@param x? number The initial x position (optional)
---@param y? number The initial y position (optional)
---
---@return Transform transform The lua transform object
---If this function fails, you will get errors as well as a transform object with a nil _c_component pointer.
---If unspecified, the initial position will be (0, 0).
---
---example:
---```lua
---local player = Entity:getEntityNamed("PLAYER")
---player.Transform = Transform:new(player)
---player.Transform = Transform:new(player, 100, 200)
---```
function Transform:addTransform(entity, x, y)
    -- create the transform itself
    local transform = {}
    setmetatable(transform, Transform_mt)

    -- track onto its parent
    transform.parent = entity

    if x and y then
        transform._c_component = ye_lua_create_transform(entity._c_entity, x, y)
    else
        transform._c_component = ye_lua_create_transform(entity._c_entity, 0, 0)
    end

    entity.Transform = transform

    return transform
end

----------------------------------------------



-------------- SETUP / CLEANUP ---------------

-- Make Entity and Transform global
_G["Entity"] = Entity
_G["Transform"] = Transform

log("debug", "bootstrapped runtime.lua onto new VM\n")
-- print("runtime version:",YE_LUA_RUNTIME_VERSION)

----------------------------------------------