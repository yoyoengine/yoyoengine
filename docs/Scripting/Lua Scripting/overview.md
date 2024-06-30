---
tags:
  - lua
---

# Lua scripting overview

Lua scripting is the easiest way to get into making games with yoyoengine.

This guide assumes you have already setup a basic yoyoengine project, check out the [getting started guide](../../../Getting Started/getting_started) if you haven't already.

## Intellisense

If you're using Visual Studio Code, you can enable generic Lua intellisense through the [sumneko extension](https://github.com/LuaLS/lua-language-server).

To enable yoyoengine specific intellisense, you need to make the extension aware of the yoyoengine Lua runtime.

yoyoengine prebuilds a single ~2k line file containing the entirely of the Lua runtime, placed inside `yoyoengine/editor/build/{PLATFORM}/engine_resources/ye_runtime.lua`.

If you have this file open in any window in Visual Studio Code, the language server will automatically incorporate it's functions into the intellisense.

Alternatively, you can manually include the runtime file in your `.vscode/settings.json` like so:

```json
{
    "Lua.workspace.library": [
        "{ABSOLUTE PATH TO yoyoengine/editor/build/bin/{PLATFORM}/engine_resources/ye_runtime.lua}",
    ],
}
```

!!! note
    As I'm writing this, I adjusted the path in `yoyoengine/.vscode/settings.json` to be generic, but I don't think it applies when you're attatched to a workspace that isn't the root of `yoyoengine`.

## Creating an empty script

In yoyoeditor, select an entity you wish to attach a script to, and select the "Script" tab under the "Components:" selector.

You will see a message stating "No script component", and a text input with the label "Script Path:".

As of writing, the path you enter must ***already exist*** somewhere in your `resources` directory.

Once you have created a script file (it can be empty), you can enter the path relative to the `resources` directory in the text input.

For example, if you created a script file at `game/resources/scripts/my_script.lua`, you would enter `scripts/my_script.lua` into the text input.

Press the "Add Script Component" button, then save the project with `Ctrl+S`, and the script will be created when you run the game.

## Callbacks

The core of Lua scripting in yoyoengine is the use of callbacks.

Callbacks are functions recognized by yoyoengine automatically from inside your scripts, which implicity register engine behavior.

To see what I mean, open your new script file and add the following code:

```lua
function onMount()
    log("info", "Hello, yoyoengine!\n")
end
```

Save the file, rebuild and run the game. You should see in the console output:

```txt
[2024-06-30 09:56:34] [INFO] [LUA]:  Hello, yoyoengine!
```

`onMount` is a callback function that runs once when the script is attached to an entity.

yoyoengine will automatically detect the signature of this function, and call it as needed. How nice!

!!! note
    Callbacks are the main method of executing lua script code, but it's important to note that any code you put outside of a callback function ***is*** still executed, but only when the file is first loaded. This is useful for things like declaring global variables.

## Callbacks List

Similarly to `onMount`, there are a bunch of other callbacks you can use in your scripts.

### onUnmount

You might want to sit down for this one.

```lua
function onUnmount()
    log("info", "Goodbye, yoyoengine!\n")
end
```

`onUnmount` is called before the script is removed from an entity.

### onUpdate

```lua
function onUpdate()
    log("info", "I'm updating!\n")
end
```

`onUpdate` is called once, every frame.

To be more precise, it's called after:

1. Deltatime updates
2. Any new scene loads
3. Engine timers update
4. Input system runs
5. Physics system runs
6. Trick (plugin) system runs

But before the frame is actually rendered.

See `engine/src/engine.c:ye_process_frame()` for more information.

### onCollision

`onCollision` is the callback that runs when a physics collision occurs between ***any*** two ***static*** collider entities in the scene.

```lua
function onCollision(collider, other)
    log("info", collider.name .. " collided with " .. other.name .. "!\n")
end
```

`collider` is the entity that had velocity applied to it, and `other` is the entity that was collided with when the velocity was applied.

These are both representations of the Lua `Entity` table, and as such you can call any valid yoyoengine Lua api functions on them.

### onTriggerEnter

`onTriggerEnter` is called when a physics collision occurs between any ***static*** collider, and any ***trigger*** collider entities in the scene.

```lua
function onTriggerEnter(collider, other)
    log("info", collider.name .. " entered trigger " .. other.name .. "!\n")
end
```

`collider` is the entity that entered the trigger, and `other` is the trigger entity that was entered.

And again, these are both representations of the Lua `Entity` table.

## Sending Data Between Scripts

Often, you will find yourself wanting to run functions in other scripts, potentially retrieving data from them. Or, you might also want to read or write global variables in other scripts states.

Thankfully, I'm a genuis and used chatgpt to help brainstorm a solution for this.

The `LuaScript` table is a class dedicated to communication and manipulation of Lua script components in yoyoengine.

The prototypes for the class functions are as follows:

```lua
---**Attempt to call a function from another state with vargs**
---@param sig string
---@vararg any Arguments to pass to the function
---@return any
function LuaScript:Invoke(sig, ...) end

---**Get a GLOBAL value from another state**
---@param key string The name of the global
---@return any
function LuaScript:Get(key) end

---**Set a GLOBAL value in another state**
---The value must already exist.
---@param key string The name of the global
---@param value any The value to set
function LuaScript:Set(key, value) end
```

These prototypes are linked to the `LuaScript` field of the `Entity` table, so they are actual invokable methods on any entity object.

### Example

Imagine a script `script1.lua`, attatched to an entity named `script1`:

```lua
-- Script 1

SomeGlobal = 0

function GlobalPlusAmount(amount)
    return SomeGlobal + amount
end
```

If we were to make any new script in the same scene, we could run something like this:

```lua
-- Any other script in same scene

local script1 = Entity:getEntityNamed("script1")

log("info", "SomeGlobal is " .. script1.LuaScript:Get("SomeGlobal") .. "\n")

script1.LuaScript:Set("SomeGlobal", 5)

log("info", "SomeGlobal is now " .. script1.LuaScript:Get("SomeGlobal") .. "\n")

local plusfive = script1.LuaScript:Invoke("GlobalPlusAmount", 5)

log("info", "GlobalPlusAmount(5) is " .. plusfive .. "\n")
```

You would observe the following output in the console when executing the script:

```txt
[2024-06-30 09:56:34] [INFO] [LUA]:  SomeGlobal is 0
[2024-06-30 09:56:34] [INFO] [LUA]:  SomeGlobal is now 5
[2024-06-30 09:56:34] [INFO] [LUA]:  GlobalPlusAmount(5) is 10
```

!!! tip
    This feature works with varags as well, so you can pass any number of arguments to the function you're invoking. Additionally, you can return multiple return values from the function you're calling.

All of this in combination makes for a powerful system to communicate between scripts in yoyoengine.
