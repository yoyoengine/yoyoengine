--[[
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
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