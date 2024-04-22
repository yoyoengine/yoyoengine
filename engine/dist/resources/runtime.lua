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
    | This file is a critical component of the yoyoengine lua runtime.               |
    | It creates an object oriented interface to the data oriented yoyoengine C API. |
    | This file will be bundled into engine.yep upon all game builds.                |
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
local function validateEntity(self)
    if self._c_entity == nil then
        log("error", "Entity method called for nil entity\n")
        return false
    end
    return true
end

----------------------------------------------



---------------- ENTITY TABLE ----------------

---@class Entity
---@field _c_entity lightuserdata
---@field Transform Transform
---@field Camera Camera
---@field Renderer Renderer
Entity = {
    --- reference to the C pointer
    ---@type lightuserdata
    _c_entity = nil,
    
    -- references to the components
    
    ---@type Transform
    Transform = nil, 

    ---@type Camera
    Camera = nil,

    ---@type Renderer
    Renderer = nil,





    ---**Get the active state of the entity.**
    ---
    ---@return boolean state The active state of the entity
    ---
    ---example:
    ---```lua
    ---local prop_box = Entity:newEntity("BOX")
    ---print("box is active: ", prop_box:getActive())
    ---```
    getActive = function(self)
        if not validateEntity(self) then
            log("error", "Entity:getActive called for nil entity, returning false!\n")
            return false
        end

        return ye_lua_ent_get_active(self._c_entity)
    end,





    ---**Set the active state of the entity.**
    ---
    ---@param state boolean The desired active state
    ---
    ---example:
    ---```lua
    ---local prop_box = Entity:newEntity("BOX")
    ---prop_box:setActive(true)
    ---```
    setActive = function(self, state)
        -- log("info", "Entity:setActive called\n")

        if not validateEntity(self) then
            return
        end

        -- check if state is not boolean
        if type(state) ~= "boolean" then
            log("error", "Entity:setActive called with non-boolean state\n")
            return
        end

        ye_lua_ent_set_active(self._c_entity, state)
    end,





    ---**Get the ECS ID number of the entity.**
    ---
    ---@return integer ID The ECS ID number of the entity (-1 for failure)
    ---
    ---example:
    ---```lua
    ---local prop_box = Entity:newEntity("BOX")
    ---print("box ID: ", prop_box:getID())
    ---```
    getID = function(self)
        if not validateEntity(self) then
            return -1
        end

        return ye_lua_ent_get_id(self._c_entity)
    end,





    ---**Set the name of the entity.**
    ---
    ---@param name string The desired name of the entity
    ---
    ---example:
    ---```lua
    ---local prop_box = Entity:newEntity("BOX")
    ---prop_box:setName("BOX")
    ---```
    setName = function(self, name)
        if not validateEntity(self) then
            return
        end

        -- check if name is not string
        if type(name) ~= "string" then
            log("error", "Entity:setName called with non-string name\n")
            return
        end

        ye_lua_ent_set_name(self._c_entity, name)
    end,





    ---**Get the name of the entity.**
    ---
    ---@return string name The name of the entity
    ---
    ---example:
    ---```lua
    ---local prop_box = Entity:newEntity("BOX")
    ---print("box name: ", prop_box:getName())
    ---```
    getName = function(self)
        if not validateEntity(self) then
            return "ERROR"
        end

        return ye_lua_ent_get_name(self._c_entity)
    end,
}



-- define the entity metatable
Entity_mt = {
    __index = Entity
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
        entity._c_entity = ye_lua_create_entity(name)
    else
        entity._c_entity = ye_lua_create_entity()
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
    entity._c_entity = ye_lua_ent_get_entity_named(name)
    if entity._c_entity == nil then
        log("error", "Entity:getEntityNamed failed to find entity\n")
        -- for now lets keep initializing because its better to create meta for
        -- a broken entity, than to return a nil entity, since the script has a
        -- better chance of recovery
    end

    -- TODO: YOU SHOULD AUTOMATICALLY GET COMPONENTS AND SUCH HERE

    return entity
end

----------------------------------------------



-------------- TRANSFORM TABLE ---------------

---@class Transform
---@field parent Entity
---@field _c_component lightuserdata
---@field x number The x position
---@field y number The y position
Transform = {
    ---@type Entity
    parent = nil,

    ---@type lightuserdata
    _c_component = nil,
}



-- define the transform metatable
Transform_mt = {
    __index = function(self, key) 
        local parent = rawget(self, "parent")

        if not validateEntity(parent) then
            log("error", "Transform field accessed on nil/null entity\n")
            return nil
        end

        local x = ye_lua_transform_get_position_x(parent._c_entity)
        local y = ye_lua_transform_get_position_y(parent._c_entity)

        if key == "x" then
            return x
        elseif key == "y" then
            return y
        else
            log("error", "Transform field accessed with invalid key\n")
            return nil
        end
    end,

    __newindex = function(self, key, value)
        local parent = rawget(self, "parent")

        if not validateEntity(parent) then
            log("error", "Transform field accessed on nil/null entity\n")
            return
        end

        if key == "x" then
            ye_lua_transform_set_position_x(parent._c_entity, value)
        elseif key == "y" then
            ye_lua_transform_set_position_y(parent._c_entity, value)
        else
            log("error", "Transform field accessed with invalid key\n")
            return
        end
    end,
}



---**Create a new transform component.**
---
---@param entity Entity The entity to attach the transform to
---@param x? number The initial x position (optional)
---@param y? number The initial y position (optional)
---
---@return Transform transform The lua transform object
---If this function fails, you will get errors as well as a transform object with a nil _c_component pointer.
---If unspecified, the initial position will be (0, 0).
---
---example:
---```lua
---local player = Entity:getEntityNamed("PLAYER")
---player.Transform = Transform:new(player)
---player.Transform = Transform:new(player, 100, 200)
---```
function Transform:addTransform(entity, x, y)
    -- create the transform itself
    local transform = {}
    setmetatable(transform, Transform_mt)

    -- track onto its parent
    rawset(transform, "parent", entity)

    if x and y then
        --transform._c_component = 
        ye_lua_create_transform(entity._c_entity, x, y)
    else
        --transform._c_component = 
        ye_lua_create_transform(entity._c_entity, 0, 0)
    end

    entity.Transform = transform

    return transform
end

----------------------------------------------



--------------- CAMERA TABLE -----------------

---@class Camera
---@field parent Entity
---@field _c_component lightuserdata
---@field isActive boolean Controls whether the camera is active
---@field isRelative boolean Controls whether the position of the camera is relative to a root transform
---@field z number The z index
---@field x number The x position
---@field y number The y position
---@field w number The width
---@field h number The height
Camera = {
    ---@type Entity
    parent = nil,

    ---@type lightuserdata
    _c_component = nil,

    -- The "fields" dont need listed here
    -- because they are actually stored in
    -- C, and just abstracted through metatable
    -- methods
}



-- define the camera metatable
Camera_mt = {
    __index = function(self, key)
        local parent = rawget(self, "parent")
        if not validateEntity(parent) then
            log("error", "Camera field accessed on nil/null entity\n")
            return nil
        end

        -- TODO: IF YOU ADD THIS TYPE OF ACCESS TO ENTITY CLASS THIS WILL OVERFLOW
        local isActive, isRelative, z, x, y, w, h = ye_lua_camera_query(parent._c_entity)

        if key == "isActive" then
            return isActive
        elseif key == "isRelative" then
            return isRelative
        elseif key == "z" then
            return z
        elseif key == "x" then
            return x
        elseif key == "y" then
            return y
        elseif key == "w" then
            return w
        elseif key == "h" then
            return h
        else
            log("error", "Camera field accessed with invalid key\n")
            return nil
        end
    end,

    __newindex = function(self, key, value)
        if not validateEntity(self.parent) then
            log("error", "Camera field accessed on nil/null entity\n")
            return
        end

        if key == "isActive" then
            ye_lua_camera_modify(self.parent._c_entity, value, nil, nil, nil, nil, nil, nil)
        elseif key == "isRelative" then
            ye_lua_camera_modify(self.parent._c_entity, nil, value, nil, nil, nil, nil, nil)
        elseif key == "z" then
            ye_lua_camera_modify(self.parent._c_entity, nil, nil, value, nil, nil, nil, nil)
        elseif key == "x" then
            ye_lua_camera_modify(self.parent._c_entity, nil, nil, nil, value, nil, nil, nil)
        elseif key == "y" then
            ye_lua_camera_modify(self.parent._c_entity, nil, nil, nil, nil, value, nil, nil)
        elseif key == "w" then
            ye_lua_camera_modify(self.parent._c_entity, nil, nil, nil, nil, nil, value, nil)
        elseif key == "h" then
            ye_lua_camera_modify(self.parent._c_entity, nil, nil, nil, nil, nil, nil, value)
        else
            log("error", "Camera field accessed with invalid key\n")
            return
        end
    end,
}



---**Create a new camera component.**
---
---@param entity Entity The entity to attach the camera to
---@param x number The x position of the camera
---@param y number The y position of the camera
---@param w number The width of the camera
---@param h number The height of the camera
---@param z number The z index of the camera
---
---@return Camera | nil camera The lua camera object
function Camera:addCamera(entity, x, y, w, h, z)
    -- create the camera itself
    local camera = {}
    setmetatable(camera, Camera_mt)

    -- track onto its parent
    -- camera.parent = entity
    rawset(camera, "parent", entity)

    if x and y and w and h and z then
        ye_lua_create_camera(entity._c_entity, x, y, w, h, z)
    else
        log("error", "Camera:addCamera called with missing parameters\n")
        return nil
    end

    entity.Camera = camera
    -- TODO: rawset

    return camera
end



----------------------------------------------



-------------- RENDERER TABLE ----------------

---@enum RendererAligmment The alignment of the renderer
RendererAlignment = {
    TOP_LEFT = 0,
    TOP_CENTER = 1,
    TOP_RIGHT = 2,
    MID_LEFT = 3,
    MID_CENTER = 4,
    MID_RIGHT = 5,
    BOT_LEFT = 6,
    BOT_CENTER = 7,
    BOT_RIGHT = 8,
    STRETCH = 9
}

---@enum RendererType The type of the renderer
RendererType = {
    TEXT = 0,
    TEXT_OUTLINED = 1,
    IMAGE = 2,
    ANIMATION = 3,
    TILE = 4
}

---@class Image The image renderer impl
---@field parent Entity
---@field src string
Image = {
    ---@type Entity
    parent = nil,
}

Image_mt = {
    __index = function(self, key)

        local parent = rawget(self, "parent")

        if not validateEntity(parent) then
            log("error", "Image field accessed on nil/null entity\n")
            return nil
        end

        local src = ye_lua_image_renderer_query(parent._c_entity)

        if key == "src" then
            return src
        else
            log("error", "Image field accessed with invalid key\n")
            return nil
        end
    end,

    __newindex = function(self, key, value)

        local parent = rawget(self, "parent")

        if not validateEntity(parent) then
            log("error", "Image field accessed on nil/null entity\n")
            return
        end

        if key == "src" then
            ye_lua_image_renderer_modify(self.parent._c_entity, value)
        else
            log("error", "Image field accessed with invalid key\n")
            return
        end
    end,
}

---@class Text The text renderer impl
---@field parent Entity
---@field text string
---@field fontName string
---@field colorName string
---@field fontSize number
---@field wrapWidth number
Text = {
    ---@type Entity
    parent = nil,
}

Text_mt = {
    __index = function(self, key)

        local parent = rawget(self, "parent")

        if not validateEntity(parent) then
            log("error", "Text field accessed on nil/null entity\n")
            return nil
        end

        local text, fontName, fontSize, colorName, wrapWidth = ye_lua_text_renderer_query(parent._c_entity)

        if key == "text" then
            return text
        elseif key == "fontName" then
            return fontName
        elseif key == "fontSize" then
            return fontSize
        elseif key == "colorName" then
            return colorName
        elseif key == "wrapWidth" then
            return wrapWidth
        else
            log("error", "Text field accessed with invalid key\n")
            return nil
        end
    end,

    __newindex = function(self, key, value)

        local parent = rawget(self, "parent")

        if not validateEntity(parent) then
            log("error", "Text field accessed on nil/null entity\n")
            return
        end

        -- -- check if the parent is even of type Text renderer
        -- if parent.Renderer.type ~= RendererType.TEXT then
        --     log("error", "Text field accessed on non-text renderer entity\n")
        --     return nil
        -- end
        -- REDUNDANT BECAUSE METATABLE ON RENDERER FILTERS INVALID ACCESS IN FIRST PLACE

        if key == "text" then
            ye_lua_text_renderer_modify(parent._c_entity, value, nil, nil, nil, nil)
        elseif key == "fontName" then
            ye_lua_text_renderer_modify(parent._c_entity, nil, value, nil, nil, nil)
        elseif key == "fontSize" then
            ye_lua_text_renderer_modify(parent._c_entity, nil, nil, value, nil, nil)
        elseif key == "colorName" then
            ye_lua_text_renderer_modify(parent._c_entity, nil, nil, nil, value, nil)
        elseif key == "wrapWidth" then
            ye_lua_text_renderer_modify(parent._c_entity, nil, nil, nil, nil, value)
        else
            log("error", "Text field accessed with invalid key\n")
            return
        end
    end,
}

---@class TextOutlined
---@field parent Entity
---@field text string
---@field fontName string
---@field colorName string
---@field fontSize number
---@field wrapWidth number
---@field outlineSize number
---@field outlineColorName string
TextOutlined = {
    ---@type Entity
    parent = nil,
}

TextOutlined_mt = {
    __index = function(self, key)

        local parent = rawget(self, "parent")

        if not validateEntity(parent) then
            log("error", "TextOutlined field accessed on nil/null entity\n")
            return nil
        end

        local text, fontName, fontSize, colorName, wrapWidth, outlineSize, outlineColorName = ye_lua_text_outlined_renderer_query(parent._c_entity)

        if key == "text" then
            return text
        elseif key == "fontName" then
            return fontName
        elseif key == "fontSize" then
            return fontSize
        elseif key == "colorName" then
            return colorName
        elseif key == "wrapWidth" then
            return wrapWidth
        elseif key == "outlineSize" then
            return outlineSize
        elseif key == "outlineColorName" then
            return outlineColorName
        else
            log("error", "TextOutlined field accessed with invalid key\n")
            return nil
        end
    end,

    __newindex = function(self, key, value)

        local parent = rawget(self, "parent")

        if not validateEntity(parent) then
            log("error", "TextOutlined field accessed on nil/null entity\n")
            return
        end

        if key == "text" then
            ye_lua_text_outlined_renderer_modify(parent._c_entity, value, nil, nil, nil, nil, nil, nil)
        elseif key == "fontName" then
            ye_lua_text_outlined_renderer_modify(parent._c_entity, nil, value, nil, nil, nil, nil, nil)
        elseif key == "fontSize" then
            ye_lua_text_outlined_renderer_modify(parent._c_entity, nil, nil, value, nil, nil, nil, nil)
        elseif key == "colorName" then
            ye_lua_text_outlined_renderer_modify(parent._c_entity, nil, nil, nil, value, nil, nil, nil)
        elseif key == "wrapWidth" then
            ye_lua_text_outlined_renderer_modify(parent._c_entity, nil, nil, nil, nil, value, nil, nil)
        elseif key == "outlineSize" then
            ye_lua_text_outlined_renderer_modify(parent._c_entity, nil, nil, nil, nil, nil, value, nil)
        elseif key == "outlineColorName" then
            ye_lua_text_outlined_renderer_modify(parent._c_entity, nil, nil, nil, nil, nil, nil, value)
        else
            log("error", "TextOutlined field accessed with invalid key\n")
            return
        end
    end,
}

---@class Tile
---
---The origin (0,0) of the tilemap is in the top left, and x increases to the right, y increases down.
---
---@field parent Entity
---@field handle string
---@field srcX number The (pixel) offset of the x position of the tile in the tileset
---@field srcY number The (pixel) offset of the y position of the tile in the tileset
---@field srcW number The width of the tile in the tileset
---@field srcH number The height of the tile in the tileset
Tile = {
    ---@type Entity
    parent = nil,
}

Tile_mt = {
    __index = function(self, key)

        local parent = rawget(self, "parent")

        if not validateEntity(parent) then
            log("error", "Tile field accessed on nil/null entity\n")
            return nil
        end

        local handle, srcX, srcY, srcW, srcH = ye_lua_tile_renderer_query(parent._c_entity)

        if key == "handle" then
            return handle
        elseif key == "srcX" then
            return srcX
        elseif key == "srcY" then
            return srcY
        elseif key == "srcW" then
            return srcW
        elseif key == "srcH" then
            return srcH
        else
            log("error", "Tile field accessed with invalid key\n")
            return nil
        end
    end,

    __newindex = function(self, key, value)

        local parent = rawget(self, "parent")

        if not validateEntity(parent) then
            log("error", "Tile field accessed on nil/null entity\n")
            return
        end

        if key == "handle" then
            ye_lua_tile_renderer_modify(parent._c_entity, value, nil, nil, nil, nil)
        elseif key == "srcX" then
            ye_lua_tile_renderer_modify(parent._c_entity, nil, value, nil, nil, nil)
        elseif key == "srcY" then
            ye_lua_tile_renderer_modify(parent._c_entity, nil, nil, value, nil, nil)
        elseif key == "srcW" then
            ye_lua_tile_renderer_modify(parent._c_entity, nil, nil, nil, value, nil)
        elseif key == "srcH" then
            ye_lua_tile_renderer_modify(parent._c_entity, nil, nil, nil, nil, value)
        else
            log("error", "Tile field accessed with invalid key\n")
            return
        end
    end,
}

---@class Renderer
---@field parent Entity
---@field _c_component lightuserdata
---@field isActive boolean Controls whether the camera is active
---@field isRelative boolean Controls whether the position of the camera is relative to a root transform
---@field alpha number The alpha to render at
---@field z number The z index
---@field x number The x position
---@field y number The y position
---@field w number The width
---@field h number The height
---@field alignment integer The alignment of the renderer
---@field rotation number The clockwise rotation in degrees to render at
---@field flipX boolean Flip the renderer on the x axis
---@field flipY boolean Flip the renderer on the y axis
---@field preserveOriginalSize boolean Controls whether the texture grows/shrinks or stays the same size when aligining in bounds
---@field type RendererType The current type of the renderer_impl
---@field Image Image The image renderer impl
---@field Text Text The text renderer impl
---@field TextOutlined TextOutlined The text renderer impl
---@field Animation Animation The animation renderer impl
---@field Tile Tile The tile renderer impl
Renderer = {
    ---@type Entity
    parent = nil,

    ---@type lightuserdata
    _c_component = nil,

    -- TODO: REMOVEME?
    ---@type RendererAligmment
    alignment = nil,
}

Renderer_mt = {
    __index = function(self, key)
        local parent = rawget(self, "parent")
        if not validateEntity(parent) then
            log("error", "Renderer field accessed on nil/null entity\n")
            return nil
        end

        -- do all the base access on renderer fields (not impl)
        local isActive, isRelative, alpha, z, x, y, w, h, alignment, rotation, flipX, flipY, preserveOriginalSize, type = ye_lua_renderer_query(parent._c_entity)
    
        if key == "isActive" then
            return isActive
        elseif key == "isRelative" then
            return isRelative
        elseif key == "alpha" then
            return alpha
        elseif key == "z" then
            return z
        elseif key == "x" then
            return x
        elseif key == "y" then
            return y
        elseif key == "w" then
            return w
        elseif key == "h" then
            return h
        elseif key == "alignment" then
            return alignment
        elseif key == "rotation" then
            return rotation
        elseif key == "flipX" then
            return flipX
        elseif key == "flipY" then
            return flipY
        elseif key == "preserveOriginalSize" then
            return preserveOriginalSize
        elseif key == "type" then
            return type
        else
            local msg = "Renderer field access with invalid key \"" .. key .. "\". Double check the renderer type.\n"
            log("error", msg)
            return nil
        end
    end,

    __newindex = function(self, key, value)
        local parent = rawget(self, "parent")
        if not validateEntity(parent) then
            log("error", "Renderer field accessed on nil/null entity\n")
            return
        end

        if key == "isActive" then
            ye_lua_renderer_modify(parent._c_entity, value, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil)
        elseif key == "isRelative" then
            ye_lua_renderer_modify(parent._c_entity, nil, value, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil)
        elseif key == "alpha" then
            ye_lua_renderer_modify(parent._c_entity, nil, nil, value, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil)
        elseif key == "z" then
            ye_lua_renderer_modify(parent._c_entity, nil, nil, nil, value, nil, nil, nil, nil, nil, nil, nil, nil, nil)
        elseif key == "x" then
            ye_lua_renderer_modify(parent._c_entity, nil, nil, nil, nil, value, nil, nil, nil, nil, nil, nil, nil, nil)
        elseif key == "y" then
            ye_lua_renderer_modify(parent._c_entity, nil, nil, nil, nil, nil, value, nil, nil, nil, nil, nil, nil, nil)
        elseif key == "w" then
            ye_lua_renderer_modify(parent._c_entity, nil, nil, nil, nil, nil, nil, value, nil, nil, nil, nil, nil, nil)
        elseif key == "h" then
            ye_lua_renderer_modify(parent._c_entity, nil, nil, nil, nil, nil, nil, nil, value, nil, nil, nil, nil, nil)
        elseif key == "alignment" then
            ye_lua_renderer_modify(parent._c_entity, nil, nil, nil, nil, nil, nil, nil, nil, value, nil, nil, nil, nil)
        elseif key == "rotation" then
            ye_lua_renderer_modify(parent._c_entity, nil, nil, nil, nil, nil, nil, nil, nil, nil, value, nil, nil, nil)
        elseif key == "flipX" then
            ye_lua_renderer_modify(parent._c_entity, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, value, nil, nil)
        elseif key == "flipY" then
            ye_lua_renderer_modify(parent._c_entity, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, value, nil)
        elseif key == "preserveOriginalSize" then
            ye_lua_renderer_modify(parent._c_entity, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, value)
        else
            log("error", "Renderer field accessed with invalid key\n")
            return
        end
    end,
}

-- TODO: im mirroring the C constructer, in the future provide overload params that are optional
---**Create a new image renderer component.**
---
---@param entity Entity The entity to attach the renderer to
---@param handle string The path to the image to render relative to resources/
---@param z number The z index of the renderer
---
---@return Renderer renderer The lua renderer object
function Renderer:addImageRenderer(entity, handle, z)
    -- create the renderer itself
    local renderer = {}
    setmetatable(renderer, Renderer_mt)

    -- track onto its parent
    rawset(renderer, "parent", entity)

    -- create the C renderer
    ye_lua_create_image_renderer(entity._c_entity, handle, z)

    entity.Renderer = renderer -- TODO stack overflow in future, rawset

    -- create the impl and track it with rawset
    local Image = {}
    setmetatable(Image, Image_mt)

    rawset(Image, "parent", entity)

    rawset(renderer, "Image", Image)

    -- set impl type
    rawset(renderer, "type", RendererType.IMAGE)

    return renderer
end

---**Create a new text renderer component.**
---
---This text will create a width and height based on its point size,
---which can be modified through its table fields afterwards and will
---resize itself following its aligment rules in the renderer component
---
---@param entity Entity The entity to attach the renderer to
---@param text string The text to render
---@param fontName string The name of the (cached) font to use
---@param fontSize number The size of the font to use
---@param colorName string The name of the (cached) color to use
---@param z number The z index of the renderer
---
---@return Renderer renderer The lua renderer object
function Renderer:addTextRenderer(entity, text, fontName, fontSize, colorName, z)
    -- create the renderer itself
    local renderer = {}
    setmetatable(renderer, Renderer_mt)

    -- track onto its parent
    rawset(renderer, "parent", entity)

    -- create the C renderer
    ye_lua_create_text_renderer(entity._c_entity, text, fontName, fontSize, colorName, z)

    entity.Renderer = renderer -- TODO stack overflow in future, rawset

    -- create the impl and track it with rawset
    local Text = {}
    setmetatable(Text, Text_mt)

    rawset(Text, "parent", entity)

    rawset(renderer, "Text", Text)

    -- set impl type
    rawset(renderer, "type", RendererType.TEXT)

    return renderer
end

---**Create a new text outlined renderer component.**
---
---This text will create a width and height based on its point size,
---which can be modified through its table fields afterwards and will
---resize itself following its aligment rules in the renderer component
---
---@param entity Entity The entity to attach the renderer to
---@param text string The text to render
---@param fontName string The name of the (cached) font to use
---@param fontSize number The size of the font to use
---@param colorName string The name of the (cached) color to use
---@param outlineSize number The size of the outline
---@param outlineColorName string The name of the (cached) color to use for the outline
---@param z number The z index of the renderer
---
---@return Renderer renderer The lua renderer object
function Renderer:addTextOutlinedRenderer(entity, text, fontName, fontSize, colorName, outlineSize, outlineColorName, z)
    -- create the renderer itself
    local renderer = {}
    setmetatable(renderer, Renderer_mt)

    -- track onto its parent
    rawset(renderer, "parent", entity)

    -- create the C renderer
    ye_lua_create_text_outlined_renderer(entity._c_entity, text, fontName, fontSize, colorName, outlineSize, outlineColorName, z)

    entity.Renderer = renderer -- TODO stack overflow in future, rawset

    -- create the impl and track it with rawset
    local TextOutlined = {}
    setmetatable(TextOutlined, TextOutlined_mt)

    rawset(TextOutlined, "parent", entity)

    rawset(renderer, "TextOutlined", TextOutlined)

    -- set impl type
    rawset(renderer, "type", RendererType.TEXT_OUTLINED)

    return renderer
end

---**Create a new tile renderer component.**
---
---@param entity Entity The entity to attach the renderer to
---@param handle string The path to the tileset to use
---@param srcX number The (pixel) offset of the x position of the tile in the tileset
---@param srcY number The (pixel) offset of the y position of the tile in the tileset
---@param srcW number The width of the tile in the tileset
---@param srcH number The height of the tile in the tileset
---@param z number The z index of the renderer
function Renderer:addTileRenderer(entity, handle, srcX, srcY, srcW, srcH, z)
    -- create the renderer itself
    local renderer = {}
    setmetatable(renderer, Renderer_mt)

    -- track onto its parent
    rawset(renderer, "parent", entity)

    -- create the C renderer
    ye_lua_create_tile_renderer(entity._c_entity, handle, srcX, srcY, srcW, srcH, z)

    entity.Renderer = renderer -- TODO stack overflow in future, rawset

    -- create the impl and track it with rawset
    local Tile = {}
    setmetatable(Tile, Tile_mt)

    rawset(Tile, "parent", entity)

    rawset(renderer, "Tile", Tile)

    -- set impl type
    rawset(renderer, "type", RendererType.TILE)

    return renderer
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