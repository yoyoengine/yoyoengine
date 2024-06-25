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

---@class Input
--- Exposes interfaces to the input subsystem
Input = {}

---MouseState represents the state of the mouse.
---@class MouseState
---@field x number The x-coordinate of the mouse.
---@field y number The y-coordinate of the mouse.
---@field leftClicked boolean Whether the left mouse button is clicked.
---@field middleClicked boolean Whether the middle mouse button is clicked.
---@field rightClicked boolean Whether the right mouse button is clicked.

---**Get the mouse state**
---
---@return MouseState
---example:
---```lua
---print("mouse x:" .. Input:mouse().x)
---```
function Input:mouse()
    return ye_lua_input_query_mouse()
end

function Input:keyPressed(key)
    return ye_lua_input_query_key(true, key) -- true means its keycode
end

function Input:scanPressed(scan)
    return ye_lua_input_query_key(false, scan) -- false means its scancode
end

function Input:keyMod(mod)
    return ye_lua_input_query_mod(mod)
end