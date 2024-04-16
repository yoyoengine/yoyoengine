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

-- define the Entity class
Entity = {
    -- reference to the C pointer
    _c_entity = nil,

    -- references to the components
    Transform = nil, 
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

-- API : create a new entity
function Entity:new()
    -- create the entity itself
    local entity = {}
    setmetatable(entity, Entity_mt)

    -- create the transform component
    entity.Transform = {}
    setmetatable(entity.Transform, Transform_mt)

    -- TODO : call the C API to create a new entity
    return entity
end

-- Make Entity and Transform global
_G["Entity"] = Entity
_G["Transform"] = Transform

log("debug", "bootstrapped runtime.lua onto new VM\n")