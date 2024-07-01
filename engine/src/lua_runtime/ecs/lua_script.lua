--[[
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
]]

---@class LuaScript
---@field isActive boolean Controls whether the LuaScript is active
---@field scriptHandle string The resource handle the script was loaded from
LuaScript = {
    -- no **real** fields.
    -- This exists purely for intellisense
}

local LuaScriptIndexer = {
    isActive = 1,
    scriptHandle = 2,
}

---------------------------------------------------------
---             INITIALIZE SCRIPT METHODS             ---
---------------------------------------------------------

---**Attempt to call a function from another state with vargs**
---@param sig string
---@vararg any Arguments to pass to the function
---@return any
function LuaScript:Invoke(sig, ...) end -- fake prototype for intellisense
function Invoke(entity, sig, ...)
    local parent_ptr = rawget(entity, "parent_ptr")
    return ye_invoke_cross_state_function(parent_ptr, sig, ...)
end

---**Get a GLOBAL value from another state**
---@param key string The name of the global
---@return any
function LuaScript:Get(key) end -- fake prototype for intellisense
function Get(entity, key)
    local parent_ptr = rawget(entity, "parent_ptr")
    return ye_read_cross_state_value(parent_ptr, key)
end

---**Set a GLOBAL value in another state**
---The value must already exist.
---@param key string The name of the global
---@param value any The value to set
function LuaScript:Set(key, value) end -- fake prototype for intellisense
function Set(entity, key, value)
    local parent_ptr = rawget(entity, "parent_ptr")
    return ye_write_cross_state_value(parent_ptr, key, value)
end

local LuaScriptMethods = {
    ["Invoke"] = Invoke,
    ["Get"] = Get,
    ["Set"] = Set,
}

---------------------------------------------------------

LuaScript_mt = {
    __index = function(self, key)

        -- Lookup key in method table, return method if found
        if LuaScriptMethods[key] then
            return LuaScriptMethods[key]
        end

        return ValidateAndQuery(self, key, LuaScriptIndexer, ye_lua_lua_script_query, "LuaScript")
    end,

    __newindex = function(self, key, value)
        return ValidateAndModify(self, key, value, LuaScriptIndexer, ye_lua_lua_script_modify, "LuaScript")
    end,
}

---**Create a new LuaScript component.**
---
---@param handle string The resource handle of the script to load
function Entity:AddLuaScriptComponent(handle) end -- fake prototype for intellisense
function AddLuaScriptComponent(self, handle)
    if handle then
        Entity:addComponent(self, ye_lua_create_lua_script, handle)
    else
        log("error", "AddLuaScriptComponent called with missing parameters\n")
    end
end

---**Remove the LuaScript component from the entity.**
function Entity:RemoveLuaScriptComponent() end -- fake prototype for intellisense
function RemoveLuaScriptComponent(self)
    ye_lua_remove_component(rawget(self, "_c_entity"), LUASCRIPT_COMPONENT)
end