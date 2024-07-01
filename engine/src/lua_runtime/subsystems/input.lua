--[[
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
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

---ControllerState represents the state of a controller.
---@class ControllerState
---@field a boolean A button
---@field b boolean B button
---@field x boolean X button
---@field y boolean Y button
---@field start boolean Start button
---@field back boolean Back button
---@field guide boolean Guide button
---@field leftStick boolean Left stick button
---@field rightStick boolean Right stick button
---@field leftShoulder boolean Left shoulder button
---@field rightShoulder boolean Right shoulder button
---@field dPadUp boolean D-pad up button
---@field dPadDown boolean D-pad down button
---@field dPadLeft boolean D-pad left button
---@field dPadRight boolean D-pad right button
---@field leftStickX number The x-coordinate of the left stick.
---@field leftStickY number The y-coordinate of the left stick.
---@field rightStickX number The x-coordinate of the right stick.
---@field rightStickY number The y-coordinate of the right stick.
---@field leftTrigger number The left trigger value.
---@field rightTrigger number The right trigger value.
---@field name string The name of the controller.
---@field attached boolean Whether the controller is currently connected.

---REMOVED: @field mapping string The mapping of the controller.

---**Get the state of a controller**
---
---@param controllerNum number The controller number.
---@return ControllerState
---example:
---```lua
---print("controller 0 a button pressed:" .. Input:controller(0).a)
---```
function Input:controller(controllerNum)
    return ye_lua_input_query_controller(controllerNum)
end

---**Get the number of connected controllers**
---
---@return number
---example:
---```lua
---print("number of connected controllers:" .. Input:controllerCount())
---```
function Input:controllerCount()
    return ye_lua_input_number_of_controllers()
end