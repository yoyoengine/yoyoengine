# engine TODO

- Need to think about how much of it is a graphics engine vs a game engine.
  - Most callback stuff is located in SCDG game code right now, and not actually in the engine.
  - Furthermore, do we want it to be a visual novel game engine strictly? That would involve a lot of code-moving to the engine from SCDG

- cpython allow framework to be used to create visual novels easily in python?
  - lua approach require c but lua build games on top
  - this decision depends on what the actual goals for the engine are. graphics vs game engine vs visual novel engine

- cmake?

- lua wrap one C function to dispatch lua calls to C, that way we dont have to rewrite every function

ive decided. its a visual novel engine that provides the extensibility to do other things and scenes with it, you have a lot of presets and options but do not have to stick to them. you also get custom lua scripting provided by the data files

cpython wrap engine so can use whole thing from python?

Nuklear fix weird input handle lag

- also design way to add custom overlays from json or other structure (does engine allow Nuklear functionality to users or is this just for editor?)

graphics file needs cleaned up pretty bad

new thread for timing delays, we can send a request with a callback ptr and it will do the awaiting on new thread

animate the yoyo engine logo spinning on bootup

stabilize the framework api before moving on to editor and python bindings

timing functions

- other thread for delay callbacks
- perf benchmarking

## console

- needs dynamic resizing
- needs command execution
  - merge most game code into this as framework (scene loading, etc)
- needs keybind to open/close
  - global state in engine.c tracking booleans which rest of engine can observe as internal state (paintbounds, overlay on, etc)
- remember where console was before it was hidden

completely rename engine function names

render from the active camera POV - math!!
engine use ECS splashscreen
port the old renderer stuff to the new ECS
cleanup all old renderer code

for ECS it would be nice to pass NULL to get some sensible defaults on some comps

would be nice to abbreviate a bunch of ECS function calls into a backwards compatible ye_create_text ye_create_image ye_create_animation etc

left off:
splash screen too big i think its bounds exceed the camera because its a larger texture. we need to fit it into the camera bounds

input state machine

timers and input not tied to frame rate...

named entities

port all the old renderer code to the new ECS and rip out old code
macro helpers for old style constructors

port interactions to ecs

sensible caching of fonts colors images automatically

- this deserves its own full fledged cache system
- possibly in the future this evolves into memory management + cache system

particle emitter that just emits entities with transforms and physics components (super lighweight)

fix debug log, should place next to exe by default but pass parm
