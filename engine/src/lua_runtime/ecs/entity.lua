--[[
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
]]

-- table to lookup Entity methods
local EntityMethods = {
    --- ADDITION FUNCTIONS
    ["AddButtonComponent"] = AddButtonComponent,
    ["AddCameraComponent"] = AddCameraComponent,
    ["AddTransformComponent"] = AddTransformComponent,
    ["AddImageRendererComponent"] = AddImageRendererComponent,
    ["AddTextRendererComponent"] = AddTextRendererComponent,
    ["AddTextOutlinedRendererComponent"] = AddTextOutlinedRendererComponent,
    ["AddTileRendererComponent"] = AddTileRendererComponent,
    ["AddAnimationRendererComponent"] = AddAnimationRendererComponent,
    ["AddTagComponent"] = AddTagComponent,
    ["AddColliderComponent"] = AddColliderComponent,
    ["AddPhysicsComponent"] = AddPhysicsComponent,
    ["AddLuaScriptComponent"] = AddLuaScriptComponent,

    --- REMOVAL FUNCTIONS
    ["RemoveButtonComponent"] = RemoveButtonComponent,
    ["RemoveCameraComponent"] = RemoveCameraComponent,
    ["RemoveTransformComponent"] = RemoveTransformComponent,
    ["RemoveRendererComponent"] = RemoveRendererComponent,
    ["RemoveTagComponent"] = RemoveTagComponent,
    ["RemoveColliderComponent"] = RemoveColliderComponent,
    ["RemovePhysicsComponent"] = RemovePhysicsComponent,
    ["RemoveLuaScriptComponent"] = RemoveLuaScriptComponent,
}

---@class Entity
---@field _c_entity lightuserdata
---
---@field Transform Transform
---@field Camera Camera
---@field Renderer Renderer
---@field Button Button
---@field Tag Tag
---@field Collider Collider
---@field Physics Physics
---@field LuaScript LuaScript
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

local function wrapComponentAccess(_c_entity, compIdx, mt)
    if not ye_lua_check_component_exists(_c_entity, compIdx) then return nil end
    return CreateProxyToComponent(_c_entity, mt)
end

-- define the entity metatable
Entity_mt = {
    __index = function(self, key)
        local _c_entity = rawget(self, "_c_entity")

        -- if we are accessing _c_entity, return it
        if key == "_c_entity" then
            return _c_entity
        end

        -- I have actually zero idea why these cannot go in EntityMethods,
        -- but it literally will not work if you do that lmao
        if key == "destroy" then
            return DeleteEntity
        end
        if key == "duplicate" then
            return DuplicateEntity
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
            return wrapComponentAccess(_c_entity, TRANSFORM_COMPONENT, Transform_mt)
        end
        if key == "Renderer" then
            return wrapComponentAccess(_c_entity, RENDERER_COMPONENT, Renderer_mt)
        end

        if key == "Camera" then
            return wrapComponentAccess(_c_entity, CAMERA_COMPONENT, Camera_mt)
        end

        if key == "LuaScript" then
            return wrapComponentAccess(_c_entity, LUASCRIPT_COMPONENT, LuaScript_mt)
        end

        if key == "Button" then
            return wrapComponentAccess(_c_entity, BUTTON_COMPONENT, Button_mt)
        end

        if key == "Physics" then
            return wrapComponentAccess(_c_entity, PHYSICS_COMPONENT, Physics_mt)
        end

        if key == "Collider" then
            return wrapComponentAccess(_c_entity, COLLIDER_COMPONENT, Collider_mt)
        end

        if key == "Tag" then
            return wrapComponentAccess(_c_entity, TAG_COMPONENT, Tag_mt)
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

    -- compare the root C entity (since we can have any arbitrary Entity tables with the same _c_entity pointer)
    __eq = function(self, other)
        return rawget(self, "_c_entity") == rawget(other, "_c_entity")
    end
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

---**Get an entity by ID.**
---
---@param id integer The ID of the entity to get
---@return Entity entity The lua entity object
---If this function fails, you will get errors as well as an entity
---object with a nil _c_entity pointer.
---
---example:
---```lua
---local player = Entity:getEntityByID(1)
---```
function Entity:getEntityByID(id)
    -- create the entity itself
    local entity = {}
    setmetatable(entity, Entity_mt)

    -- get the _c_entity pointer
    rawset(entity, "_c_entity", ye_lua_ent_get_entity_by_id(id))
    if entity._c_entity == nil then
        log("error", "Entity:getEntityByID failed to find entity\n")
    end

    return entity
end

---**Get an entity by tag.**
---
---@param tag string The tag of the entity to get
---@return Entity entity The lua entity object
---If this function fails, you will get errors as well as an entity
---object with a nil _c_entity pointer.
---
---example:
---```lua
---local player = Entity:getEntityByTag("PLAYER")
---```
function Entity:getEntityByTag(tag)
    -- create the entity itself
    local entity = {}
    setmetatable(entity, Entity_mt)

    -- get the _c_entity pointer
    rawset(entity, "_c_entity", ye_lua_ent_get_entity_by_tag(tag))
    if entity._c_entity == nil then
        log("error", "Entity:getEntityByTag failed to find entity\n")
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
    local _c_entity = rawget(entity, "_c_entity")
    
    -- create the component in the engine
    cComponentCreationFunc(_c_entity, ...)
end

---**Destroy the entity.**
function Entity:destroy() end -- intellisense
function DeleteEntity(self)
    -- rawget the entity pointer (to avoid metatable)
    local _c_entity = rawget(self, "_c_entity")
    ye_lua_delete_entity(_c_entity)
end

---**Duplicate the entity2.**
---
---@return Entity entity The new copy of the entity
function Entity:duplicate() end -- intellisense
function DuplicateEntity(self)
    -- create the entity itself
    local entity = {}
    setmetatable(entity, Entity_mt)

    -- rawget the entity pointer (to avoid metatable)
    local _c_entity = rawget(self, "_c_entity")
    local new_c_ent = ye_lua_duplicate_entity(_c_entity)
    rawset(entity, "_c_entity", new_c_ent)

    return entity
end

---**Reference to the current entity.**
---
---@type Entity
this = {}