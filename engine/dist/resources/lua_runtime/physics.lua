--[[
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
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

---@class Physics
---@field isActive boolean Controls whether the physics component is active
---@field xVelocity number The x velocity (in pixels per second)
---@field yVelocity number  The y velocity (in pixels per second)
---@field rotationalVelocity number The rotational velocity (in degrees per second)
Physics = {
    -- no **real** fields.
    -- This exists purely for intellisense
}

local physicsIndexer = {
    isActive = 1,
    xVelocity = 2,
    yVelocity = 3,
    rotationalVelocity = 4,
}

Physics_mt = {
    __index = function(self, key)
        return ValidateAndQuery(self, key, physicsIndexer, ye_lua_physics_query, "Physics")
    end,

    __newindex = function(self, key, value)
        return ValidateAndModify(self, key, value, physicsIndexer, ye_lua_physics_modify, "Physics")
    end,
}

---**Create a new physics component.**
---
---@param xVelocity number The x velocity (in pixels per second)
---@param yVelocity number The y velocity (in pixels per second)
function Entity:AddPhysicsComponent(xVelocity, yVelocity) end -- fake prototype for intellisense
function AddPhysicsComponent(self, xVelocity, yVelocity)
        Entity:addComponent(self, ye_lua_create_physics_component, xVelocity, yVelocity)
end

---**Remove the physics component.**
function Entity:RemovePhysicsComponent() end -- fake prototype for intellisense
function RemovePhysicsComponent(self)
    ye_lua_remove_component(rawget(self, "_c_entity"), PHYSICS_COMPONENT)
end