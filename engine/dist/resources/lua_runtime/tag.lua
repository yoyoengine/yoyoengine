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

---------------------------------------------------------

---@class Tag
---@field _c_component lightuserdata
---@field isActive boolean Controls whether the tag comp is active
Tag = {
    -- no **real** fields.
    -- This exists purely for intellisense
}

local tagIndexer = {
    isActive = 1,
}


---------------------------------------------------------
---               INITIALIZE TAG METHODS              ---
---------------------------------------------------------

---**Check if the entity has a tag.**
---@param tag string
---@return boolean
function Tag:HasTag(tag) end -- fake prototype for intellisense
function HasTag(entity, tag)
    local parent_ptr = rawget(entity, "parent_ptr")
    return ye_lua_tag_has_tag(parent_ptr, tag)
end

---**Add a tag to the entity.**
---@param tag string
function Tag:AddTag(tag) end -- fake prototype for intellisense
function AddTag(entity, tag)
    local parent_ptr = rawget(entity, "parent_ptr")
    ye_lua_tag_add_tag(parent_ptr, tag)
end

---**Remove a tag from the entity.**
---@param tag string
function Tag:RemoveTag(tag) end -- fake prototype for intellisense
function RemoveTag(entity, tag)
    local parent_ptr = rawget(entity, "parent_ptr")
    ye_lua_tag_remove_tag(parent_ptr, tag)
end

local TagMethods = {
    ["HasTag"] = HasTag,
    ["AddTag"] = AddTag,
    ["RemoveTag"] = RemoveTag,
}

---------------------------------------------------------

-- define the tag metatable
Tag_mt = {
    __index = function(self, key)

        -- Lookup key in method table, return method if found
        if TagMethods[key] then
            return TagMethods[key]
        end

        return ValidateAndQuery(self, key, tagIndexer, ye_lua_tag_query, "Tag")
    end,

    __newindex = function(self, key, value)
        return ValidateAndModify(self, key, value, tagIndexer, ye_lua_tag_modify, "Tag")
    end,
}

---**Create a new tag component.**
function Entity:AddTagComponent() end -- fake prototype for intellisense
function AddTagComponent(self)
    Entity:addComponent(self, ye_lua_create_tag)
end