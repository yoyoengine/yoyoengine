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

---@class Timer
--- Exposes interfaces to the timer subsystem, and provides methods to access and manipulate time.
Timer = {}

---**Creates a new timer object.**
---@param duration_ms number The duration of the timer in milliseconds.
---@param callback_fn function The function to call when the timer expires.
---@param loops number The number of times to loop the timer. -1 means loop forever.
---@param start_ticks number The number of ticks to wait before starting the timer.
---@vararg any Additional arguments to pass to the callback function when resolved.
---**IMPORTANT: THERE IS A MAXIMUM CAP FOR TIMER CALLBACK ARGUMENTS, DEFINED IN ENGINE AS YE_LUA_MAX_TIMER_ARGS**
function Timer:new(duration_ms, callback_fn, loops, start_ticks, ...)
    -- loops = loops or 0
    -- start_ticks = start_ticks or -1 -- -1 makes timer system start it immediately
    -- no more optional args, because we have the varargs

    -- check we have all valid args
    if not duration_ms or not callback_fn or not loops or not start_ticks then
        log("error","Called Timer:new() without all required arguments")
    else
        ye_lua_timer_create_timer(duration_ms, callback_fn, loops, start_ticks, ...)
    end
end

---**Retrieves the current ticks of the engine**
---@param offset? number An offset to apply to the result
---@return number The current ticks of the engine
function Timer:getTicks(offset)
    offset = offset or 0
    return ye_lua_timer_get_ticks() + offset
end