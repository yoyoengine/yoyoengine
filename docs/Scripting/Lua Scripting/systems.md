---
tags:
  - lua
---

# Lua API Systems

The lua API provides wrappers over top of the C based yoyoengine systems, like the timer and input systems.

## Timer

The `Timer` table is a class dedicated to timekeeping and timer based callbacks in yoyoengine.

Here are some of the prototypes:

```lua
---**Retrieves the current ticks of the engine**
---@param offset? number An optional offset to apply to the result
---@return number The current ticks of the engine
function Timer:getTicks(offset) end

--**Creates a new timer object.**
---@param duration_ms number The duration of the timer in milliseconds.
---@param callback_fn function The function to call when the timer expires.
---@param loops number The number of times to loop the timer. -1 means loop forever.
---@param start_ticks number The number of ticks to wait before starting the timer.
---@vararg any Additional arguments to pass to the callback function when resolved.
---**IMPORTANT: THERE IS A MAXIMUM CAP FOR TIMER CALLBACK ARGUMENTS, DEFINED IN ENGINE AS YE_LUA_MAX_TIMER_ARGS**
function Timer:new(duration_ms, callback_fn, loops, start_ticks, ...) end
```

As indicated in the function signatures, there are caveats or special considerations for both.

### What are ticks?

The timer system operates on a system based around `SDL_Ticks`, which is a counter (in the resoulution of milliseconds), that starts at 0 when the engine is initialized and increments by 1 every millisecond.

So `Timer:getTicks()` will return `1000`, one second after the engine is initialized.

### Timer:getTicks

`getTicks` takes in an optional parameter, `offset`, which is a number of milliseconds to add to the current tick count. This is useful for calculating future or past ticks without explicitly doing the arithmetic yourself.

```lua
local ticks = Timer:getTicks()
local oneMinuteFromNow = Timer:getTicks(60000) -- 60000 milliseconds = 1 minute

log("info", "Current ticks: " .. ticks .. "\n")
log("info", "One minute from now: " .. oneMinuteFromNow .. "\n")
```

### Timer:new

`new` is a function that creates a new engine timer object. The timer object takes in:

- a duration in milliseconds
- a Lua callback function
- "loops" ie: the number of times to run the timer (which can be a little deceptive if you imagine the first execution to be outside of the loop)
- the number of ticks to wait before starting the timer
- any additional arguments to pass to the callback function.

If `loops` is -1, the timer will persist forever (until the scene changes).
If `startTicks` is <=0, the timer will start immediately.

Here's an example use case:

```lua
local function myCallback(arg1, arg2)
    log("info", "Callback called with args: " .. arg1 .. ", " .. arg2 .. "\n")
    log("info", "It is currently " .. Timer:getTicks() .. "\n")
end

log("info", "It is currently " .. Timer:getTicks() .. "\n")

local myTimer = Timer:new(1000, myCallback, 5, 0, "Hello", "World")
```

would output:

```txt
[2024-06-30 09:56:34] [INFO] [LUA]:  It is currently 0
[2024-06-30 09:56:35] [INFO] [LUA]:  Callback called with args: Hello, World
[2024-06-30 09:56:35] [INFO] [LUA]:  It is currently 1000
[2024-06-30 09:56:36] [INFO] [LUA]:  Callback called with args: Hello, World
[2024-06-30 09:56:36] [INFO] [LUA]:  It is currently 2000
[2024-06-30 09:56:37] [INFO] [LUA]:  Callback called with args: Hello, World
[2024-06-30 09:56:37] [INFO] [LUA]:  It is currently 3000
[2024-06-30 09:56:38] [INFO] [LUA]:  Callback called with args: Hello, World
[2024-06-30 09:56:38] [INFO] [LUA]:  It is currently 4000
[2024-06-30 09:56:39] [INFO] [LUA]:  Callback called with args: Hello, World
[2024-06-30 09:56:39] [INFO] [LUA]:  It is currently 5000
```

!!! warning
    Because the timer system is reliant on operating system scheduling, the timer will not be 100% accurate (like it is in this example output). It will usually be within a few dozen millis of the desired time, but if you need more accurate timing you should pursue writing your own timer system via the C scripting API. 

## Scene

A simple `Scene` class table is provided to manipulate scene data.

Currently, you are provided:

```lua
---**Load a scene by handle**
---
---@param handle string The path to the scene to load relative to resources/
---@return nil
---example:
---```lua
---ye_load_scene("scenes/game.yoyo")
---```
function Scene:loadScene(sceneName) end
```

This function is used to load a scene by handle. The handle is the path to the scene relative to the `resources/` directory.

## Input

The `Input` table provides convenient wrappers around the native C input handler over top of `SDL_Event`.

### Mouse

For mouse handling, you have:

```lua
---**Get the mouse state**
---
---@return MouseState
---example:
---```lua
---print("mouse x:" .. Input:mouse().x)
---```
function Input:mouse() end
```

which will return a table `MouseState`:

```lua
---MouseState represents the state of the mouse.
---@class MouseState
---@field x number The x-coordinate of the mouse.
---@field y number The y-coordinate of the mouse.
---@field leftClicked boolean Whether the left mouse button is clicked.
---@field middleClicked boolean Whether the middle mouse button is clicked.
---@field rightClicked boolean Whether the right mouse button is clicked.
```

### Keyboard

For keyboard handling, you get to pick between keycode and scancode based functions.
The difference between them lies in the "physical location" of the key.

I tend to forget every single time, so I would reccomend looking at this [SO thread](https://stackoverflow.com/questions/56915258/difference-between-sdl-scancode-and-sdl-keycode).

Here are your options for keyboard polling:

```lua
function Input:keyPressed(key) end

function Input:scanPressed(scan) end

function Input:keyMod(mod) end
```

`keyPressed` and `scanPressed` take in a `SDL_Keycode` and `SDL_Scancode` respectively (which are defined in `engine/src/lua_runtime/definitions/input_codes.lua`), and return a boolean indicating if the key is pressed.

`keyMod` takes in a `SDL_Keymod` (defined in the same source file) and returns a boolean indicating if the modifier is pressed.

For example, if you wanted to check if the `A` key was pressed, at the same time as the `ctrl` modifier, you would do:

```lua
if Input:keyPressed(KEY_A) and Input:keyMod(MOD_CTRL) then
    log("info", "Ctrl + A pressed\n")
end
```

If you're experiencing some weird behavior (ie: input you press not lining up with response), take a peek at `yoyoengine/engine/src/lua_runtime/definitions/input_codes.lua` and cross check it with the `SDL_Keycode` and `SDL_Scancode` definitions in SDL source.

### Controller

For controller handling, you have:

```lua
---**Get the state of a controller**
---
---@param controllerNum number The controller number.
---@return ControllerState
---example:
---```lua
---print("controller 0 a button pressed:" .. Input:controller(0).a)
---```
function Input:controller(controllerNum) end

---**Get the number of connected controllers**
---
---@return number
---example:
---```lua
---print("number of connected controllers:" .. Input:controllerCount())
---```
function Input:controllerCount() end
```

`controllerCount()` is pretty self explanatory, but controller() returns a table with the following fields:

```lua
---ControllerState represents the state of a controller.
---@class ControllerState
---@field a boolean A button
---@field b boolean B button
---@field x boolean X button
---@field y boolean Y button
---@field start boolean Start button
---@field back boolean Back button
---@field guide boolean Guide button
---@field leftStick boolean Left stick button
---@field rightStick boolean Right stick button
---@field leftShoulder boolean Left shoulder button
---@field rightShoulder boolean Right shoulder button
---@field dPadUp boolean D-pad up button
---@field dPadDown boolean D-pad down button
---@field dPadLeft boolean D-pad left button
---@field dPadRight boolean D-pad right button
---@field leftStickX number The x-coordinate of the left stick.
---@field leftStickY number The y-coordinate of the left stick.
---@field rightStickX number The x-coordinate of the right stick.
---@field rightStickY number The y-coordinate of the right stick.
---@field leftTrigger number The left trigger value.
---@field rightTrigger number The right trigger value.
---@field name string The name of the controller.
---@field attached boolean Whether the controller is currently connected.
```

Just for the sake of providing an example, if you wanted to check if the dpad up button was pressed on the 0th connected controller, you would do:

```lua
if Input:controller(0).dPadUp then
    log("info", "DPad up pressed on controller 0\n")
end
```

The stick and trigger related fields are returned as floats between -1 and 1, with 0 being the center position.

!!! tip
    A lot of controllers experience drift, so you might want to add a deadzone tolerance to your games controller input handling.

## Audio

The audio system is super simple, and provides 3 basic functions:

```lua
---**Plays a sound effect**
---@param handle string The resource handle
---@param loops? number The number of times to loop the sound (default 0)
---@param volume_scale? number The volume scale (default 1.0)
function Audio:playSound(handle, loops, volume_scale) end

---**Plays a music track**
---@param handle string The resource handle
---@param loops? number The number of times to loop the music (default 0)
---@param volume_scale? number The volume scale (default 1.0)
function Audio:playMusic(handle, loops, volume_scale) end

---**Sets the global volume of the audio subsystem**
--- The global volume scales down or up all audio output on a per channel basis
---@param volume_scale number The volume scale (default 1.0)
function Audio:setVolume(volume_scale) end
```

I'll let the function signatures speak for themselves, but just be aware of the `?` parameter syntax, which indicates an optional parameter. So, for example:

```lua
Audio:playSound("sounds/boop.wav")
```

would play the `resources/sounds/boop.wav` file once at maximum volume.

!!! note
    The audio system is configured to only accept `wav` and `mp3` files currently.
    If you want to hack in more format support, it should be pretty trivial by changing the compilation flags for `SDL_Mixer` in `yoyoengine/engine/CMakeLists.txt`
