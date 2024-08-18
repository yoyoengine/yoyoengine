---
tags:
  - c
---

# C scripting

Since I have transitioned this project to be more of an engine and less of a framework, by using the launcher to create a new project you will recieve a structure that is preconfigured to build and run a game by default.
In order to define custom behavior (ie: actually script your game) you should make note of the `custom/` directory created in your project folder.

```txt
custom
├── include
│   └── yoyo_c_api.h
└── src
    └── router.c
```

This directory is where all of your code for the game should go. `yoyo_c_api.h` defines some minimal API that interface with the engine managed entry point (`../entry.c`), while `router.c` is where you can start implementing your custom behavior.

## Headers

Any headers you put inside of `custom/include` will be automatically included in the build process. This is where you should define any custom headers you want to use in your game.

The compiler knows to look in this directory, so you can define any headers here. I dont think you can nest subdirectories though, but if you want to it should be trivial to modify the build script or you can always open an issue and id be happy to add that.

## Scripting

The fundamental principle of C scripting is registering callbacks.
Inside of `custom/src/router.c` there is (by default) a function called `void yoyo_register_callbacks()` that is setup by the entry point to run before the engine is initialized.

It should look like this:

```c
void yoyo_register_callbacks() {
    // register our input handler (persistent flag means it will never be unlinked)
    ye_register_event_cb(YE_EVENT_HANDLE_INPUT, handle_input, YE_EVENT_FLAG_PERSISTENT);

    // register our example scene load handler
    ye_register_event_cb(YE_EVENT_SCENE_LOAD, example_scene_load, YE_EVENT_FLAG_PERSISTENT);

    /*
        TIP:
        You are allowed to stack callbacks, so if you
        want multiple scene load handlers you can do that!
    */
}
```

The `ye_register_event_cb` function is from `engine/src/event.c` and is used to register a callback function pointer to any observable engine event.

```c
void ye_register_event_cb(enum ye_event_type type, void *cb, int flags);
```

Flags are optional (pass `YE_EVENT_FLAG_NONE` for default), and are used to add attributes to the callback.

For example, the `YE_EVENT_FLAG_PERSISTENT` flag is used to ensure that the callback is never unlinked from the event (even if we load a new scecne).

## Wholistic Example

To put it all together, you can use the non persistant default nature of callbacks to register scene-specific observers.

```c
void game_logic_callback() {
    // do some game logic
    printf("game logic\n");
}

void scene_load(char *scene_name) {
    if (strcmp(scene_name, "example") == 0) {
        // handle load for this specific scene
        // ...

        // now register callbacks for this scene
        ye_register_event_cb(YE_EVENT_PRE_FRAME, game_logic_callback, YE_EVENT_FLAG_NONE);
    }
}

void yoyo_register_callbacks() {
    // ...

    ye_register_event_cb(YE_EVENT_SCENE_LOAD, scene_load, YE_EVENT_FLAG_PERSISTENT);

    // ...
}
```

With this, when the `example` scene is loaded, the `game_logic_callback` will be called every frame. For other scenes, nothing will happen.

## Exposing your own API to Lua

An awesome benefit of C scripting, is that you can implement some performance intensive or hyper specific functionality in C, and then expose it to Lua.

When yoyoengine creates game objects, it iterates over a list of functions to use to register signatures into the Lua state.

You are able to hook into this as well, by subscribing to the YE_EVENT_LUA_REGISTER event.

```c
// custom C function we want to give to every Lua state
int exit_game(lua_State *L){
    YG_RUNNING = false;
    return 0;
}

void register_lua_api(lua_State *L){
    // register custom lua api
    lua_register(L, "exitGame", exit_game);
}

void yoyo_register_callbacks() {
    // ...

    ye_register_event_cb(YE_EVENT_LUA_REGISTER, register_lua_api, YE_EVENT_FLAG_PERSISTENT);

    // ...
}
```

With this, `exitGame()` is now a valid callable function in every Lua state created by the engine.

## To link libraries

With the new build system 3.0, you are given a `CMakeLists.txt` file in the root of your game template. You are free to manually modify this and link in whatever libraries you see fit.