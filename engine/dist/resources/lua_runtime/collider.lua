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