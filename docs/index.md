# Welcome to yoyoengine!

You are currently reading yoyoengine docs.

These are generated using [mkdocs-material](https://squidfunk.github.io/mkdocs-material/) from source in :simple-github:  [yoyoengine](https://github.com/zoogies/yoyoengine)

The way I have structured this project, all dependancies for the core, editor, and built distributable projects will be downloaded with FetchContent and built automatically by cmake.
**However**, you will need some external packages on your system to use some of the features and programs included within this repo, which are listed [here](#dependencies).

## Documentation

You're currently reading my hand crafted documentation, but for more advanced developers, you might want to generate doxygen documentation yourself.

To build doxygen docs, run `doxygen Doxyfile` in the `yoyoengine/engine` directory.

The examples directory in this repository contains minimal examples of usage, which can be opened through adding them as projects in the launcher gui, then opening them. (As of writing this feature is not complete).

## Notices

This section is temporary and will be used to denote any important facts I need to remember about the project.

- The coordinate system is non standard, north is -y, south is +y, east is +x, west is -x
- only text or png images can have their alpha modulated
- The engine is single threaded (except for audio)
- Logging is unavailable until the engine is initialized (SDL needs loaded before we can use it)
- THE LAUNCHER AND EDITOR TOOLCHAIN HAS ONLY BEEN TESTED ON LINUX. OPEN AN ISSUE IF YOU NEED WINDOWS SUPPORT.
- NOTE: make this section limitations and have a seperate notices
- DO NOT USE SPACES IN FILE NAMES, this will break a lot of the file operations especially in the editor, core might be fine though.
- Because of the nature of the editor, it only runs on linux and also accesses data through the loose file directories rather than the pack files which are accessed at runtime
- for buttons, their state is updated on input events, meaning that a "clicked" button would only stop being clicked on the next event NOT the next frame. We could fix this with a iteration and reset at the beginning of each frame, but for now its reset when polled. So a button will only be "clicked" for the first access of the state after the click event.
- There is a limitation in detection of the screen size. Moving games to be fullscreen on your non primary monitor will mess with the mouse position detection, as the size of your primary monitor is used to calculate fullscreen letterboxing
- We can only guarantee one timer executing its callback per frame, which is usually more than good enough unless at extremely low framerates and for time sensitive tasks. There is already a bit of loss due to CPU scheduling, so the timer system wouldnt be ideal for hyper accurate timing anyways.
- The `onMount()` lua function incrementally as the entities in the scene are created, so if you try to `Entity:getEntityNamed()` for an entity that isnt created yet you will not find it.

### editor

- check out <https://emscripten.org/docs/getting_started/downloads.html> for how to install emscripten, after that assuming you have everything else installed you can build to emscripten outputs

## Dependencies

!!! warning
    You will need all of the below dependencies installed, besides the **Core & Editor** dependencies, as they come preinstalled.

### Tooling

- A C compiler (gcc, clang, etc) (Mingw for cross compilation)
- [git](https://git-scm.com/)
- [cmake](https://cmake.org/)
- [make](https://www.gnu.org/software/make/)
- [python3](https://www.python.org/)
- [pip](https://pypi.org/project/pip/)
- [emscripten](https://emscripten.org/) (for emscripten builds)

### Core & Editor

!!! info  
    All of these libraries are included in this repository, and do not need to be installed.

- [SDL2](https://www.libsdl.org/)
- [SDL2_image](https://www.libsdl.org/projects/SDL_image/)
- [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/)
- [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/)
- [SDL2_net](https://www.libsdl.org/projects/SDL_net/)
- [uthash](https://github.com/troydhanson/uthash)
- [Nuklear](https://github.com/Immediate-Mode-UI/Nuklear)
- [jansson](https://github.com/akheron/jansson)

### Launcher / Project Manager

- [tkinter](https://docs.python.org/3/library/tkinter.html)
- [CustomTkinter](https://customtkinter.tomschimansky.com/)
- [CTkMessagebox](https://github.com/Akascape/CTkMessagebox)

## Launcher

There is a project manager and editor launcher in the "editor" directory. This is currently a work in progress and requires some python tkinter deps to be installed, but if you run it, it should function. It is currently only tested on linux.

You can create, delete, and launch projects through this easily.

## Editor

!!! warning
    The editor has only been tested on linux, and is incomplete. IT WILL NOT RUN ON WINDOWS!!!

## Plans

Some features that aren't planned but I would like to implement eventually:

- [ ] Networking (Stretch for after 1.0)
- [ ] Implement a Cpython wrapper
- [X] Bundling assets into production data files
- [ ] 3D rendering
- [ ] Mac support
