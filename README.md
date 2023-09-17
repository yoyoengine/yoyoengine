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

## Dependencies

- [SDL2](https://www.libsdl.org/)
- [SDL2_image](https://www.libsdl.org/projects/SDL_image/)
- [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/)
- [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/)
- [uthash](https://github.com/troydhanson/uthash)
- [Nuklear](https://github.com/Immediate-Mode-UI/Nuklear)

## Documentation

- The docs are a work in progress as I iron out basically everything for this project

## Plans

- [X] Text, Images, Animations, Sound, Buttons
- [X] Lua integration
- [ ] Lua C bindings
- [X] Immediate mode gui integration
- [ ] Editor
- [ ] Collision detection
- [ ] Networking
- [ ] Example projects and tutorials
- [ ] Documentation website

## Credit

- The dependencies and Jojo's Bizarre Adventure for the bootup screen sound effect.
