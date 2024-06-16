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

---@class Transform
---@field x number The x position
---@field y number The y position
Transform = {
    -- no **real** fields.
    -- This exists purely for intellisense
}

-- define the transform metatable
Transform_mt = {
    __index = function(self, key) 
        local parent_ptr = rawget(self, "parent_ptr")

        -- if not ValidateEntity(parent) then
        --     log("error", "Transform field accessed on nil/null entity\n")
        --     return nil
        -- end

        local x = ye_lua_transform_get_position_x(parent_ptr)
        local y = ye_lua_transform_get_position_y(parent_ptr)

        if key == "x" then
            return x
        elseif key == "y" then
            return y
        else
            log("error", "Transform field accessed with invalid key\n")
            return nil
        end
    end,

    __newindex = function(self, key, value)
        local parent_ptr = rawget(self, "parent_ptr")

        -- if not ValidateEntity(parent) then
        --     log("error", "Transform field accessed on nil/null entity\n")
        --     return
        -- end

        if key == "x" then
            ye_lua_transform_set_position_x(parent_ptr, value)
        elseif key == "y" then
            ye_lua_transform_set_position_y(parent_ptr, value)
        else
            log("error", "Transform field accessed with invalid key\n")
            return
        end
    end,
}

---**Create a new Transform component.**
---
--- You must pass either x AND y or neither.
---
---@param x? number The x position of the Transform
---@param y? number The y position of the Transform
function Entity:AddTransformComponent(x, y) end -- fake prototype for intellisense
function AddTransformComponent(self, x, y)
    if x and y then
        Entity:addComponent(self, ye_lua_create_transform, x, y)
    else
        Entity:addComponent(self, ye_lua_create_transform, 0, 0)
    end
end

---**Remove the Transform component from the entity.**
function Entity:RemoveTransformComponent() end -- fake prototype for intellisense
function RemoveTransformComponent(self)
    ye_lua_remove_component(rawget(self, "_c_entity"), TRANSFORM_COMPONENT)
end