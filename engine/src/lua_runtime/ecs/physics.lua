--[[
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
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