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