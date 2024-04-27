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

        if not ValidateEntity(parent) then
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

        if not ValidateEntity(parent) then
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

local TextIndexer = {
    text = 1,
    fontName = 2,
    fontSize = 3,
    colorName = 4,
    wrapWidth = 5,
}

Text_mt = {
    __index = function(self, key)

        local parent = rawget(self, "parent")

        if not ValidateEntity(parent) then
            log("error", "Text field accessed on nil/null entity\n")
            return nil
        end

        local queryResult = {ye_lua_text_renderer_query(parent._c_entity)}

        if TextIndexer[key] then
            return queryResult[TextIndexer[key]]
        else
            log("error", "Text field accessed with invalid key\n")
            return nil
        end
    end,

    __newindex = function(self, key, value)

        local parent = rawget(self, "parent")

        if not ValidateEntity(parent) then
            log("error", "Text field accessed on nil/null entity\n")
            return
        end

        -- -- check if the parent is even of type Text renderer
        -- if parent.Renderer.type ~= RendererType.TEXT then
        --     log("error", "Text field accessed on non-text renderer entity\n")
        --     return nil
        -- end
        -- REDUNDANT BECAUSE METATABLE ON RENDERER FILTERS INVALID ACCESS IN FIRST PLACE

        local args = {self.parent._c_entity, nil, nil, nil, nil, nil}

        if TextIndexer[key] then
            args[TextIndexer[key] + 1] = value
            ye_lua_text_renderer_modify(Yunpack(args))
            return
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

local TextOutlinedIndexer = {
    text = 1,
    fontName = 2,
    fontSize = 3,
    colorName = 4,
    wrapWidth = 5,
    outlineSize = 6,
    outlineColorName = 7,
}

TextOutlined_mt = {
    __index = function(self, key)

        local parent = rawget(self, "parent")

        if not ValidateEntity(parent) then
            log("error", "TextOutlined field accessed on nil/null entity\n")
            return nil
        end

        local queryResult = {ye_lua_text_outlined_renderer_query(parent._c_entity)}

        if TextOutlinedIndexer[key] then
            return queryResult[TextOutlinedIndexer[key]]
        else
            log("error", "TextOutlined field accessed with invalid key\n")
            return nil
        end
    end,

    __newindex = function(self, key, value)

        local parent = rawget(self, "parent")

        if not ValidateEntity(parent) then
            log("error", "TextOutlined field accessed on nil/null entity\n")
            return
        end

        local args = {self.parent._c_entity, nil, nil, nil, nil, nil, nil, nil}

        if TextOutlinedIndexer[key] then
            args[TextOutlinedIndexer[key] + 1] = value
            ye_lua_text_outlined_renderer_modify(Yunpack(args))
            return
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

local TileIndexer = {
    handle = 1,
    srcX = 2,
    srcY = 3,
    srcW = 4,
    srcH = 5,
}

Tile_mt = {
    __index = function(self, key)

        local parent = rawget(self, "parent")

        if not ValidateEntity(parent) then
            log("error", "Tile field accessed on nil/null entity\n")
            return nil
        end

        local queryResult = {ye_lua_tile_renderer_query(parent._c_entity)}

        if TileIndexer[key] then
            return queryResult[TileIndexer[key]]
        else
            log("error", "Tile field accessed with invalid key\n")
            return nil
        end
    end,

    __newindex = function(self, key, value)

        local parent = rawget(self, "parent")

        if not ValidateEntity(parent) then
            log("error", "Tile field accessed on nil/null entity\n")
            return
        end

        local args = {self.parent._c_entity, nil, nil, nil, nil, nil}

        if TileIndexer[key] then
            args[TileIndexer[key] + 1] = value
            ye_lua_tile_renderer_modify(Yunpack(args))
            return
        else
            log("error", "Tile field accessed with invalid key\n")
            return
        end
    end,
}

--- ik ur ass is coming back to overhaul this later, so hi lol

---@class Animation
---@field parent Entity
---@field paused boolean
---@field metaFile string
---@field frameDelay number
---@field currentFrame number
---@field frameCount number Should probably not be changed directly, unless you know what youre doing
---@field frameWidth number Should probably not be changed directly, unless you know what youre doing
---@field frameHeight number Should probably not be changed directly, unless you know what youre doing
---@field imageHandle string Should probably not be changed directly, unless you know what youre doing
Animation = {
    ---@type Entity
    parent = nil,
}

local AnimationIndexer = {
    paused = 1,
    metaFile = 2,
    frameDelay = 3,
    currentFrame = 4,
    frameCount = 5,
    frameWidth = 6,
    frameHeight = 7,
    imageHandle = 8
}

Animation_mt = {
    __index = function(self, key)

        local parent = rawget(self, "parent")

        if not ValidateEntity(parent) then
            log("error", "Animation field accessed on nil/null entity\n")
            return nil
        end

        local queryResult = {ye_lua_animation_renderer_query(parent._c_entity)}

        if AnimationIndexer[key] then
            return queryResult[AnimationIndexer[key]]
        else
            log("error", "Animation field accessed with invalid key\n")
            return nil
        end
    end,

    __newindex = function(self, key, value)

        local parent = rawget(self, "parent")

        if not ValidateEntity(parent) then
            log("error", "Animation field accessed on nil/null entity\n")
            return
        end

        local args = {self.parent._c_entity, nil, nil, nil, nil, nil, nil, nil, nil}

        if AnimationIndexer[key] then
            args[AnimationIndexer[key] + 1] = value
            ye_lua_animation_renderer_modify(Yunpack(args))
            return
        else
            log("error", "Animation field accessed with invalid key\n")
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

local RendererIndexer = {
    isActive = 1,
    isRelative = 2,
    alpha = 3,
    z = 4,
    x = 5,
    y = 6,
    w = 7,
    h = 8,
    alignment = 9,
    rotation = 10,
    flipX = 11,
    flipY = 12,
    preserveOriginalSize = 13,
    type = 14
}

Renderer_mt = {
    __index = function(self, key)
        local parent = rawget(self, "parent")
        if not ValidateEntity(parent) then
            log("error", "Renderer field accessed on nil/null entity\n")
            return nil
        end

        -- do all the base access on renderer fields (not impl)
        local queryResult = {ye_lua_renderer_query(parent._c_entity)}
    
        if RendererIndexer[key] then
            return queryResult[RendererIndexer[key]]
        else
            local msg = "Renderer field access with invalid key \"" .. key .. "\". Double check the renderer type.\n"
            log("error", msg)
            return nil
        end
    end,

    __newindex = function(self, key, value)
        local parent = rawget(self, "parent")
        if not ValidateEntity(parent) then
            log("error", "Renderer field accessed on nil/null entity\n")
            return
        end

        print("setting", key, "to", value)

        local args = {parent._c_entity, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil}
        -- local fake_args = {parent._c_entity, 1, 2, nil, 4, 5, 6}

        -- print("fake args", Yunpack(fake_args))
        
        if RendererIndexer[key] then
            -- print("got arg index",RendererIndexer[key])
            args[RendererIndexer[key] + 1] = value
            -- print("args", Yunpack(args), "length of args:", #args)
            -- print("SET ARG:",args[RendererIndexer[key] + 1])
            ye_lua_renderer_modify(Yunpack(args))
            return
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

---**Create a new animation renderer component.**
---
---@param entity Entity The entity to attach the renderer to
---@param metaFile string The path to the animation meta file to use
---@param z number The z index of the renderer
function Renderer:addAnimationRenderer(entity, metaFile, z)
    -- create the renderer itself
    local renderer = {}
    setmetatable(renderer, Renderer_mt)

    -- track onto its parent
    rawset(renderer, "parent", entity)

    -- create the C renderer
    ye_lua_create_animation_renderer(entity._c_entity, metaFile, z)

    entity.Renderer = renderer -- TODO stack overflow in future, rawset

    -- create the impl and track it with rawset
    local Animation = {}
    setmetatable(Animation, Animation_mt)

    rawset(Animation, "parent", entity)

    rawset(renderer, "Animation", Animation)

    -- set impl type
    rawset(renderer, "type", RendererType.ANIMATION)

    return renderer
end