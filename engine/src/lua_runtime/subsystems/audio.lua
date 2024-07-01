--[[
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
]]

---@class Audio
--- The audio subsystem, used for playing sound effects and music
Audio = {}

---**Plays a sound effect**
---@param handle string The resource handle
---@param loops? number The number of times to loop the sound (default 0)
---@param volume_scale? number The volume scale (default 1.0)
function Audio:playSound(handle, loops, volume_scale)
    -- set optional params to defaults as needed
    loops = loops or 0
    volume_scale = volume_scale or 1.0

    -- call the engine function
    ye_audio_play_sound(handle, loops, volume_scale)
end

---**Plays a music track**
---@param handle string The resource handle
---@param loops? number The number of times to loop the music (default 0)
---@param volume_scale? number The volume scale (default 1.0)
function Audio:playMusic(handle, loops, volume_scale)
    -- set optional params to defaults as needed
    loops = loops or 0
    volume_scale = volume_scale or 1.0

    -- call the engine function
    ye_audio_play_music(handle, loops, volume_scale)
end

---**Sets the global volume of the audio subsystem**
--- The global volume scales down or up all audio output on a per channel basis
---@param volume_scale number The volume scale (default 1.0)
function Audio:setVolume(volume_scale)
    -- call the engine function
    ye_audio_set_volume(volume_scale)
end