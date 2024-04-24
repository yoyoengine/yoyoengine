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

---@class Transform
---@field parent Entity
---@field _c_component lightuserdata
---@field x number The x position
---@field y number The y position
Transform = {
    ---@type Entity
    parent = nil,

    ---@type lightuserdata
    _c_component = nil,
}

-- define the transform metatable
Transform_mt = {
    __index = function(self, key) 
        local parent = rawget(self, "parent")

        if not ValidateEntity(parent) then
            log("error", "Transform field accessed on nil/null entity\n")
            return nil
        end

        local x = ye_lua_transform_get_position_x(parent._c_entity)
        local y = ye_lua_transform_get_position_y(parent._c_entity)

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
        local parent = rawget(self, "parent")

        if not ValidateEntity(parent) then
            log("error", "Transform field accessed on nil/null entity\n")
            return
        end

        if key == "x" then
            ye_lua_transform_set_position_x(parent._c_entity, value)
        elseif key == "y" then
            ye_lua_transform_set_position_y(parent._c_entity, value)
        else
            log("error", "Transform field accessed with invalid key\n")
            return
        end
    end,
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
    rawset(transform, "parent", entity)

    if x and y then
        --transform._c_component = 
        ye_lua_create_transform(entity._c_entity, x, y)
    else
        --transform._c_component = 
        ye_lua_create_transform(entity._c_entity, 0, 0)
    end

    entity.Transform = transform

    return transform
end