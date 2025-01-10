--[[
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
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

---**Remove the tag component from the entity.**
function Entity:RemoveTagComponent() end -- fake prototype for intellisense
function RemoveTagComponent(self)
    ye_lua_remove_component(rawget(self, "_c_entity"), TAG_COMPONENT)
end