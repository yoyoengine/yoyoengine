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

-- table to lookup Entity methods
local EntityMethods = {
    ["AddButtonComponent"] = AddButtonComponent,
    ["AddCameraComponent"] = AddCameraComponent,
    ["AddTransformComponent"] = AddTransformComponent,
    ["AddImageRendererComponent"] = AddImageRendererComponent,
    ["AddTextRendererComponent"] = AddTextRendererComponent,
    ["AddTextOutlinedRendererComponent"] = AddTextOutlinedRendererComponent,
    ["AddTileRendererComponent"] = AddTileRendererComponent,
    ["AddAnimationRendererComponent"] = AddAnimationRendererComponent,
}

---@class Entity
---@field _c_entity lightuserdata
---
---@field Transform Transform
---@field Camera Camera
---@field Renderer Renderer
---@field Button Button
---
---@field isActive boolean
---@field ID integer
---@field name string
Entity = {
    --- reference to the C pointer
    ---@type lightuserdata
    _c_entity = nil,
    
    -- COMPONENTS ARE "VIRTUAL" FIELDS intercepted by
    -- the Entity metatable. We redirect them into api
    -- bridge calls to be validated, which is inefficient
    -- for non existant components but we can optimize later...
}

-- define the entity metatable
Entity_mt = {
    __index = function(self, key)
        local _c_entity = rawget(self, "_c_entity")

        -- if we are accessing _c_entity, return it
        if key == "_c_entity" then
            return _c_entity
        end

        if _c_entity == nil then
            log("error", "Entity field read on nil entity\n")
            return nil
        end

        -- Lookup key in method table, return method if found
        if EntityMethods[key] then
            return EntityMethods[key]
        end

        -- Entity Component Fields:

        if key == "Transform" then
            return CreateProxyToComponent(_c_entity, Transform_mt)
        end

        if key == "Camera" then
            return CreateProxyToComponent(_c_entity, Camera_mt)
        end

        if key == "Renderer" then
            return CreateProxyToComponent(_c_entity, Renderer_mt)
        end

        if key == "Button" then
            return CreateProxyToComponent(_c_entity, Button_mt)
        end

        -- Entity fields:

        if key == "isActive" then
            return ye_lua_ent_get_active(_c_entity) 
        end

        if key == "ID" then
            return ye_lua_ent_get_id(_c_entity)
        end

        if key == "name" then
            return ye_lua_ent_get_name(_c_entity)
        end
        log("error", "Entity field accessed with invalid key \"" .. key .. "\"\n")
    end,

    __newindex = function(self, key, value)
        local _c_entity = rawget(self, "_c_entity")

        if _c_entity == nil then
            log("error", "Attempted to modify field on nil entity\n")
            return nil
        end

        if key == "isActive" then
            -- check if state is not boolean
            if type(value) ~= "boolean" then
                log("error", "Tried to modify entity.isActive with non-boolean state\n")
                return
            end
            
            ye_lua_ent_set_active(_c_entity, value)
            return
        end

        if key == "name" then
            -- check if name is not string
            if type(value) ~= "string" then
                log("error", "Tried to modify entity.name with non-string name\n")
                return
            end
            
            ye_lua_ent_set_name(_c_entity, value)
            return
        end

        -- we arent allowed to modify ID.

        log("error", "Entity field modified with invalid key\n")
    end,
}

---**Create a new entity.**
---
---@param name? string The name of the entity to create (optional)
---@return Entity entity The lua entity object
---If this function fails, you will get errors as well as an entity
---object with a nil _c_entity pointer.
---
---example:
---```lua
---local prop_box = Entity:newEntity("BOX")
---```
function Entity:new(name)
    -- name is optional TODO: clean thhis up and add behavior

    -- create the entity itself
    local entity = {}
    setmetatable(entity, Entity_mt)

    -- get the _c_entity pointer
    if name then
        rawset(entity, "_c_entity", ye_lua_create_entity(name))
    else
        rawset(entity, "_c_entity", ye_lua_create_entity())
    end

    return entity
end

---**Get a scene entity by name.**
---
---@param name string The name of the entity to get
---@return Entity entity The lua entity object
---If this function fails, you will get errors as well as an entity
---object with a nil _c_entity pointer.
---
---example:
---```lua
---local player = Entity:getEntityNamed("PLAYER")
---```
function Entity:getEntityNamed(name)
    -- create the entity itself
    local entity = {}
    setmetatable(entity, Entity_mt)

    -- get the _c_entity pointer
    rawset(entity, "_c_entity", ye_lua_ent_get_entity_named(name))
    if entity._c_entity == nil then
        log("error", "Entity:getEntityNamed failed to find entity\n")
        -- for now lets keep initializing because its better to create meta for
        -- a broken entity, than to return a nil entity, since the script has a
        -- better chance of recovery
    end

    -- TODO: YOU SHOULD AUTOMATICALLY GET COMPONENTS AND SUCH HERE

    return entity
end

--- Generic Component Addition
---
--- This function will create and assign a component of given
--- componentType, as well as pass varags to the constructer in C
---
---@param entity Entity The entity to attach the component to
---@param cComponentCreationFunc function The function to create the component in C
---@vararg ... The arguments to pass to the C component creation function
function Entity:addComponent(entity, cComponentCreationFunc, ...)
    -- rawget the entity pointer (to avoid metatable)
    print("about to add component")
    local _c_entity = rawget(entity, "_c_entity")
    
    -- create the component in the engine
    cComponentCreationFunc(_c_entity, ...)
end