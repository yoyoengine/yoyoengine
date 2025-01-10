--[[
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
]]

---@class Collider
---@field isActive boolean Controls whether the camera is active
---@field isRelative boolean Controls whether the position of the camera is relative to a root transform
---@field x number The x position
---@field y number The y position
---@field w number The width
---@field h number The height
---@field isTrigger boolean Whether the collider is a trigger
Collider = {
    -- no **real** fields.
    -- This exists purely for intellisense
}

local colliderIndexer = {
    isActive = 1,
    isRelative = 2,
    x = 3,
    y = 4,
    w = 5,
    h = 6,
    isTrigger = 7,
}

-- define the collider metatable
Collider_mt = {
    __index = function(self, key)
        return ValidateAndQuery(self, key, colliderIndexer, ye_lua_collider_query, "Collider")
    end,

    __newindex = function(self, key, value)
        return ValidateAndModify(self, key, value, colliderIndexer, ye_lua_collider_modify, "Collider")
    end,
}

---**Create a new collider component.**
---
---@param isTrigger boolean Whether the collider is a trigger
---@param x number The x position of the collider
---@param y number The y position of the collider
---@param w number The width of the collider
---@param h number The height of the collider
function Entity:AddColliderComponent(isTrigger, x, y, w, h) end -- fake prototype for intellisense
function AddColliderComponent(self, isTrigger,  x, y, w, h)
    if isTrigger then
        Entity:addComponent(self, ye_lua_create_trigger_collider, x, y, w, h)
    else
        Entity:addComponent(self, ye_lua_create_static_collider, x, y, w, h)
    end
end

---**Remove the collider component from the entity.**
function Entity:RemoveColliderComponent() end -- fake prototype for intellisense
function RemoveColliderComponent(self)
    ye_lua_remove_component(rawget(self, "_c_entity"), COLLIDER_COMPONENT)
end