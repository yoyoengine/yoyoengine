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

---@class Camera
---@field parent Entity
---@field _c_component lightuserdata
---@field isActive boolean Controls whether the camera is active
---@field isRelative boolean Controls whether the position of the camera is relative to a root transform
---@field z number The z index
---@field x number The x position
---@field y number The y position
---@field w number The width
---@field h number The height
Camera = {
    ---@type Entity
    parent = nil,

    ---@type lightuserdata
    _c_component = nil,

    -- The "fields" dont need listed here
    -- because they are actually stored in
    -- C, and just abstracted through metatable
    -- methods
}

local cameraIndexer = {
    isActive = 1,
    isRelative = 2,
    z = 3,
    x = 4,
    y = 5,
    w = 6,
    h = 7,
}

-- define the camera metatable
Camera_mt = {
    __index = function(self, key)
        return ValidateAndQuery(self, key, cameraIndexer, ye_lua_camera_query, "Camera")
    end,

    __newindex = function(self, key, value)
        return ValidateAndModify(self, key, value, cameraIndexer, ye_lua_camera_modify, "Camera")
    end,
}

---**Create a new camera component.**
---
---@param entity Entity The entity to attach the camera to
---@param x number The x position of the camera
---@param y number The y position of the camera
---@param w number The width of the camera
---@param h number The height of the camera
---@param z number The z index of the camera
---
---@return Camera | nil camera The lua camera object
function Camera:addCamera(entity, x, y, w, h, z)
    if x and y and w and h and z then
        return Entity:addComponent("Camera", Camera_mt, ye_lua_create_camera, x, y, w, h, z)
    else
        log("error", "Camera:addCamera called with missing parameters\n")
        return nil
    end
end