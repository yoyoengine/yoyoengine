---
tags:
  - c
---

# Tricks (Plugin/Module System)

!!! warning
    The structure and metadata requirements for tricks are still very subject to change.

Plugins are supported on both Windows and Linux, but not Emscripten. Currently, they are not interchangible because the game links against them at compile time, but in the future, it is planned to have a system where plugins can be loaded at runtime (ie: modders can inject code freely).

Plugins are called "tricks". They live in subdirectories of the project folder `/tricks` and have a directory structure like this:

```txt
example/
├── include
├── lib
├── src
│   └── main.c
└── trick.yoyo
```

`trick.yoyo` contains the metadata for the trick, which is required to load it. It is a json file with the following structure:

```json
{
    "name":"example_trick",
    "description":"example trick",
    "author":"Ryan Zmuda",
    "version":"1.0"
}
```

The developer of each trick is responsible for implementing a `CMakeLists.txt` build file, which incorporates the output target directories discussed in a few paragraphs.

In order to setup a trick to be loaded, you should `#include <yoyoengine/yoyo_trick_boilerplate.h>` in your source, and implement the `yoyo_trick_init` function.

It is reccomended (required if you wish for the engine to automatically callback into your trick) to call `ye_register_trick` with a `struct ye_trick_node` that contains any metadata and callbacks you wish for the engine to be aware of.

You can also leverage the power of `ye_register_trick_lua_bindings(lua_State *state)` to push to EVERY lua state that is created by the engine any functions you wish. This is useful for exposing your own lua api to user scripts.

A minimal example of a complete trick can be seen below:

```c
#include <yoyoengine/yoyoengine.h>
#include <yoyoengine/yoyo_trick_boilerplate.h>

void yoyo_trick_on_load(){
    /*
        runs when the trick is first mounted, which happens after all other init
    */
}

void yoyo_trick_on_unload(){
    /*
        runs when the trick is about to be unloaded, which is one of the
        first things to happen on engine shutdown
    */
}

void yoyo_trick_on_update(){
    /*
        runs once every ye_process_frame call, before the lua script
        callbacks and before rendering
    */
}

/* Example function that we will expose to lua */
void test_lua_bind(lua_State *L){
    ye_logf(warning, "TEST LUA BIND\n");
}

/* Callback to register our test function */
void yoyo_trick_lua_bind(lua_State *L){
    lua_register(L, "example_test", test_lua_bind);
}

/* The only required signature in this file, entry point to our trick */
void yoyo_trick_init(){
    ye_register_trick((struct ye_trick_node){
        .name = "example",
        .author = "Ryan Zmuda",
        .description = "example trick",
        .version = "1.0.0",
        .on_load = yoyo_trick_on_load,
        .on_unload = yoyo_trick_on_unload,
        .on_update = yoyo_trick_on_update,
        .lua_bind = yoyo_trick_lua_bind
    });
}
```

The engine at build time will make a few assumptions about your trick:

- The trick folder is named the same thing as the CMake target
- It has a valid `trick.yoyo` in its root
- It has a valid `CMakeLists.txt` in its root
- It's `CMakeLists.txt` is setup in a way to build a shared library into `YOYO_TRICK_BUILD_DIR`
- Any headers that you want to be accessable to the runtime are in an `include/` directory in the root of the trick

I've deliberately left it up to the developer to create their own build script, as it gives much more control over dependancy management and build settings in general.

Because of this, it is also up to the developer to make sure they are outputting the trick in the directory that the engine will be looking for during project build time, which is `YOYO_TRICK_BUILD_DIR`.

You can ensure you are building into the correct directory by adding the following to your `CMakeLists.txt`:

```cmake
set_target_properties(example PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YOYO_TRICK_BUILD_DIR}) # linux
set_target_properties(example PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${YOYO_TRICK_BUILD_DIR}) # windows
```

You need ***BOTH*** of these properties to account for windows AND linux builds.

!!! warning
    Because tricks load after initialization and are unloaded shortly before shutdown, **it is imperative that their exposed functionality is not attempted to be invoked outside the duration of its lifecycle**. This is not enforced by the engine, and will result in undefined behavior if violated.
