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

---@class Button
---@field parent Entity
---@field _c_component lightuserdata
---@field isActive boolean Controls whether the camera is active
---@field isRelative boolean Controls whether the position of the camera is relative to a root transform
---@field x number The x position
---@field y number The y position
---@field w number The width
---@field h number The height
---@field isHovered boolean Whether the button is hovered or not
---@field isPressed boolean Whether the button is pressed down
---@field isClicked boolean Whether the button was released after a press for a "click"
Button = {
    -- no **real** fields.
    -- This exists purely for intellisense
}

local buttonIndexer = {
    isActive = 1,
    isRelative = 2,
    x = 3,
    y = 4,
    w = 5,
    h = 6,
    -- isHovered = 7,
    -- isPressed = 8,
    -- isClicked = 9,
}

Button_mt = {
    __index = function(self, key)

        local parent_ptr = rawget(self, "parent_ptr")

        -- intercept isHovered, isPressed, and isClicked
        if key == "isHovered" then
            return ye_lua_button_check_state(parent_ptr, 1)
        elseif key == "isPressed" then
            return ye_lua_button_check_state(parent_ptr, 2)
        elseif key == "isClicked" then
            return ye_lua_button_check_state(parent_ptr, 3)
        end

        return ValidateAndQuery(self, key, buttonIndexer, ye_lua_button_query, "Button")
    end,

    __newindex = function(self, key, value)
        -- modifying isXXXX is illegal, at least for now.
        
        return ValidateAndModify(self, key, value, buttonIndexer, ye_lua_button_modify, "Button")
    end,
}

---**Create a new button component.**
---
---@param x number The x position of the button
---@param y number The y position of the button
---@param w number The width of the button
---@param h number The height of the button
function Entity:AddButtonComponent(x, y, w, h) end -- fake prototype for intellisense
function AddButtonComponent(self, x, y, w, h)
    if x and y and w and h then
        Entity:addComponent(self, ye_lua_create_button, x, y, w, h)
    end
end