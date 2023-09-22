# Yoyo Engine (WIP)

A 2D graphics engine (currently framework) with optional visual novel engine capabilities, written in C using SDL2, SDL2_image, SDL2_ttf, and SDL2_mixer. All dependecies besides a C compiler and standard libary are included in this repository.

![Drawing](https://github.com/Yoyolick/SCDG/assets/43967290/f98545d1-c4ba-419e-a674-da436f591d23)

## Goals

- allow usage as
  - a framework in C
  - a framework with python bindings
- an engine through an editor
  - some sort of scripting integration
    - lua?
    - C#?
    - native C scripting?
- optional visual novel features that can be opted into as scene presets
  - maybe this takes the form of easily sharable scenes exportable into presets

## Notices

This section is temporary and will be used to denote any important facts I need to remember about the project.

- The coordinate system is non standard, north is -y, south is +y, east is +x, west is -x
- only text or png images can have their alpha modulated

## Dependencies

### Core & Editor

- [SDL2](https://www.libsdl.org/)
- [SDL2_image](https://www.libsdl.org/projects/SDL_image/)
- [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/)
- [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/)
- [uthash](https://github.com/troydhanson/uthash)
- [Nuklear](https://github.com/Immediate-Mode-UI/Nuklear)

### Launcher / Project Manager

- [tkinter](https://docs.python.org/3/library/tkinter.html)
- [CustomTkinter](https://customtkinter.tomschimansky.com/)
- [CTkMessagebox](https://github.com/Akascape/CTkMessagebox)

## Documentation

## Launcher

There is a project manager and editor launcher in the "editor" directory. This is currently a work in progress and requires some python tkinter deps to be installed, but if you run it, it should function. It is currently only tested on linux.

You can create, delete, and launch projects through this easily.

## Editor

The editor is not yet implemented, but will be a visual editor for the engine. It will essentially be a "game" running the core framework to provide an easy way to layout scenes, and tools to build them together without writing the framework interfacing code yourself.

## Framework

The framework is implemented in C, and is planned to have a Cpython wrapper available as well. Extensive docs to the framework API will be linked here when complete.

## Plans

These plans are currently sorted in order of my anticipated completion.

- [X] Basic engine functionality (Audio, Rendering, Text, etc)
- [X] Nuklear UI integration
- [x] Entity Component System
- [ ] Collision detection
- [ ] Implement native C scripting
- [ ] Decide on a non native (lua, C#) scripting language and implement it
- [ ] Stabilize the core framework API
- [ ] Serialization
- [ ] Implement a Cpython wrapper
- [ ] Visual Editor
- [ ] Overhaul the Project Manager
- [ ] Example projects and tutorials
- [ ] Documentation website
- [ ] Networking (Stretch for after 1.0)

Some features that aren't planned but I would like to implement eventually:

- [ ] Bundling assets into production data files
- [ ] 3D rendering
- [ ] Mac support
- [ ] VR support?

## Credit

- The dependencies and Jojo's Bizarre Adventure for the bootup screen sound effect.
