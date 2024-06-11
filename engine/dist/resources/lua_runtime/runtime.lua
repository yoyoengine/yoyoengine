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

--[[
    +--------------------------------------------------------------------------------+
    | This file is the entry point for the yoyoengine lua runtime API.               |
    | When a new lua scripting VM is created, every file in                          |
    | engine.yep/lua_runtime will be loaded into the VM and executed.                |
    +--------------------------------------------------------------------------------+
    | yoyoengine lua runtime version 0 - Ryan Zmuda, 2024                            |
    +--------------------------------------------------------------------------------+
--]]
YE_LUA_RUNTIME_VERSION = 0
YE_LUA_RUNTIME_AUTHOR  = "Ryan Zmuda"

--[[
    NOTES:
    - could mark some vars as private but then we cant access them across components...
]]

-------------- HELPER FUNCTIONS --------------

--- helper function that ensures _c_entity is not nil
---@param self table The entity object
---@return boolean True if the entity is valid, false otherwise
function ValidateEntity(self)
    if self._c_entity == nil then
        log("error", "Entity method called for nil entity\n")
        return false
    end
    return true
end

--- Helper function to get the highest integer key in a table
---@param t table The table
---@return number The highest integer key
local function highestKey(t)
    local max = 0
    for k in pairs(t) do
        if type(k) == "number" and k > max then
            max = k
        end
    end
    return max
end

--- Custom unpack function which does not truncate nil values
---@param t table The table to unpack
---@param i? number The starting index
---@param n? number The ending index
---@return any The unpacked values
function Yunpack(t, i, n)

    -- print the table thats going to be unpacked for debug purposes
    -- print("unpacking table")
    -- for k, v in pairs(t) do
        -- print(k, v)
    -- end
    
    i = i or 1
    n = n or highestKey(t)
    
    -- print("i:", i)
    -- print("n:", n)
    -- print("----------")

    if i <= n then
        return t[i], Yunpack(t, i + 1, n)
    end
end

--- Generic Query Function
---@param self table The entity object
---@param key any The key to query
---@param indexer table The indexer (enum) to match values with
---@param queryFunction function The function to query with
---@param name string The name of the field
function ValidateAndQuery(self, key, indexer, queryFunction, name)
    -- get the parent entity
    local parent_ptr = rawget(self, "parent_ptr")
    
    -- validate the parent entity exists
    -- if not ValidateEntity(parent_ptr) then
    --     log("error", "Failed to access " .. name .. " field on nil/null entity\n")
    --     return nil
    -- end

    -- get the query result (converted to table)
    local result = {queryFunction(parent_ptr)}

    -- return the indexed result
    if indexer[key] then
        return result[indexer[key]]
    else
        log("error", "\"" .. name .. "\" field accessed with invalid key \"" .. key .. "\". Double check the type.\n")
        return nil
    end
end

--- Generic Modify Function
---@param self table The entity object
---@param key any The key to modify
---@param value any The value to set
---@param indexer table The indexer (enum) to match values with
---@param modifyFunction function The function to modify with
---@param name string The name of the root field (not the key)
function ValidateAndModify(self, key, value, indexer, modifyFunction, name)
    -- get the parent entity
    local parent_ptr = rawget(self, "parent_ptr")

    -- -- validate the parent entity exists
    -- if not ValidateEntity(parent) then
    --     log("error", "Failed to access " .. name .. " field on nil/null entity\n")
    --     return
    -- end

    -- create the args table
    local args = {parent_ptr}
    for i = 1, #indexer do
        table.insert(args, nil) -- initialize nil args
    end

    -- set the arg value
    if indexer[key] then
        args[indexer[key] + 1] = value
        modifyFunction(Yunpack(args))
        return
    else
        log("error", "\"" .. name .. "\" field accessed with invalid key \"" .. key .. "\". Double check the type.\n")
        return
    end
end

--- Create proxy to component
--- This allows us total dispatch component field calls
--- through a component metatable while preserving the parent entity pointer
--- TODO: maybe this only needs to be called once per component to make a
---       global proxy to bypass all the overhead to do this per field
---@param parent_ptr lightuserdata The parent entity pointer
---@param metatable table The component metatable to proxy
---@return table The new proxy table
function CreateProxyToComponent(parent_ptr, metatable)
    local proxy = {}
    setmetatable(proxy, metatable)

    rawset(proxy, "parent_ptr", parent_ptr)

    return proxy
end

----------------------------------------------



-------------- SETUP / CLEANUP ---------------

-- entity is global
_G["Entity"] = Entity

-- global components
_G["Transform"] = Transform
_G["Camera"] = Camera
_G["Renderer"] = Renderer

-- global enums
_G["RendererAlignment"] = RendererAlignment
_G["RendererType"] = RendererType

log("debug", "bootstrapped runtime.lua onto new VM\n")
-- print("runtime version:",YE_LUA_RUNTIME_VERSION)

----------------------------------------------