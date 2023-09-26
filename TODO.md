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

- design way to add custom overlays from json or other structure (does engine allow Nuklear functionality to users or is this just for editor?)

new thread for timing delays, we can send a request with a callback ptr and it will do the awaiting on new thread

animate the yoyo engine logo spinning on bootup

stabilize the framework api before moving on to editor and python bindings

timing functions

- other thread for delay callbacks
- perf benchmarking

## console

- needs dynamic resizing
- needs keybind to open/close
- remember where console was before it was hidden

completely rename engine function names (hit the few remaining)

- merge most game code into this as framework (scene loading, etc)

for ECS it would be nice to pass NULL to get some sensible defaults on some comps

would be nice to abbreviate a bunch of ECS function calls into a backwards compatible ye_create_text ye_create_image ye_create_animation etc

input state machine

timers and input not tied to frame rate...

macro helpers for old style constructors

port interactions to ecs

sensible caching of fonts colors images automatically

- this deserves its own full fledged cache system
- possibly in the future this evolves into memory management + cache system

particle emitter that just emits entities with transforms and physics components (super lighweight)

need to finalize the ECS completely before i can add the mock api
once ECS is totally done we can do scene loading and execution from data files + prefab representation
after that we build basic editor
then we can build out scripting
and then its gg just polish and make a game (or a few)
networking for jackbox game

ask chatgpt how multithreading is usually done... maybe i just need a rendering thread and a game thread, what type of synchronization is needed?

The cache system could parse the scene or some top level project file declaring all used fonts and colors and then load them all at once and cache them
images should be lazily cached i think... but maybe we can get away with them not... idk

DECIDE IF YOU WANT VELOCITY AND ACCELERATION AND DRAG AND MASS OR IF
WE REALLY ONLY NEED VELOCITY

for editor panning right click
on key down right click start tracking mouse pos
on key up right click stop tracking mouse pos
calculate, etc

physics system overlay arrows of velocity

write the timing callback thread system so you dont have to SDL sleep on splash screen and you can spin the yoyo gear logo

data bundling / packing assets

scaling factor and offset for resolution

do not update physics every frame

`editor.c` TODO list

### logging before SDL init

we could use another method of getting the path for the log file.

### animations

First frame should be offset by the time it took to load the animation for accuracy?

## ECS

API needs stabilized, the lua wrapper would have an extremely hard time manipulating components because it cannot directly access the raw pointer fields

## shaping

should report in readme the things im scoping down, like the visual novel capabilities and all other features

## best practices

all functions that dont modify char \* should be const char \* (optimization and quality)
