# Yoyo Engine (WIP)

My 2D Game Engine written in C, based off of SDL.

Provides most functionality you would expect from a modern game engine, including a visual editor, launcher, and build system.

The editor projects are setup in such a way that all of your game logic can be implemented in C, or you can forgo C entirely and use lua scripting for your game logic. There is nothing stopping you from using both.

This repository comes with most library dependencies, but you will need to install some of the external dependencies listed later in this readme.

![Drawing](https://github.com/Yoyolick/SCDG/assets/43967290/f98545d1-c4ba-419e-a674-da436f591d23)

## Notices

This section is temporary and will be used to denote any important facts I need to remember about the project.

- The coordinate system is non standard, north is -y, south is +y, east is +x, west is -x
- only text or png images can have their alpha modulated
- The engine is single threaded (except for audio)
- Logging is unavailable until the engine is initialized (SDL needs loaded before we can use it)
- THE LAUNCHER AND EDITOR TOOLCHAIN HAS ONLY BEEN TESTED ON LINUX. OPEN AN ISSUE IF YOU NEED WINDOWS SUPPORT.
- NOTE: make this section limitations and have a seperate notices

### editor

- zooming too far out really messes with the SDL line renderer, and you will see weird visual artifacts.

## Dependencies

> [!WARNING]  
> You will need all of the below dependencies installed, besides the **Core & Editor** dependencies, as they come preinstalled.

### Tooling

- A C compiler (gcc, clang, etc)
- [git](https://git-scm.com/)
- [cmake](https://cmake.org/)
- [make](https://www.gnu.org/software/make/)
- [python3](https://www.python.org/)
- [pip](https://pypi.org/project/pip/)

### Core & Editor

> [!NOTE]  
> All of these libraries are included in this repository, and do not need to be installed.

- [SDL2](https://www.libsdl.org/)
- [SDL2_image](https://www.libsdl.org/projects/SDL_image/)
- [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/)
- [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/)
- [uthash](https://github.com/troydhanson/uthash)
- [Nuklear](https://github.com/Immediate-Mode-UI/Nuklear)
- [jansson](https://github.com/akheron/jansson)

### Launcher / Project Manager

- [tkinter](https://docs.python.org/3/library/tkinter.html)
- [CustomTkinter](https://customtkinter.tomschimansky.com/)
- [CTkMessagebox](https://github.com/Akascape/CTkMessagebox)

## Documentation

## Launcher

There is a project manager and editor launcher in the "editor" directory. This is currently a work in progress and requires some python tkinter deps to be installed, but if you run it, it should function. It is currently only tested on linux.

You can create, delete, and launch projects through this easily.

## Editor

> [!WARNING]  
> The editor has only been tested on linux, and is incomplete. IT WILL PROBABLY NOT RUN ON WINDOWS!!!

The editor is currently being worked on as a visual scene editor and build manager.

## Core

The core engine is implemented in C, with a possible Cpython wrapper available later on. Extensive docs to the framework API will be linked here when complete.

## Plans

These plans are currently sorted in order of my anticipated completion.

- [X] Basic engine functionality (Audio, Rendering, Text, etc)
- [X] Nuklear UI integration
- [x] Entity Component System
- [X] Serialization
- [X] Visual Editor
- [ ] Implement native C scripting
- [ ] Lua Scripting API
- [ ] Collision detection
- [ ] Stabilize the core framework API
- [ ] Overhaul the Project Manager
- [ ] Example projects and tutorials
- [ ] Documentation website

Some features that aren't planned but I would like to implement eventually:

- [ ] Networking (Stretch for after 1.0)
- [ ] Implement a Cpython wrapper
- [ ] Bundling assets into production data files
- [ ] 3D rendering
- [ ] Mac support
- [ ] VR support?

## Credit

- The listed dependencies and Jojo's Bizarre Adventure for the bootup screen sound effect.
