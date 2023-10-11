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

stabilize the framework api before moving on to editor and python bindings

timing functions

- other thread for delay callbacks
- perf benchmarking

## console

- remember where console was before it was hidden

completely rename engine function names (hit the few remaining)

for ECS it would be nice to pass NULL to get some sensible defaults on some comps

would be nice to abbreviate a bunch of ECS function calls into a backwards compatible ye_create_text ye_create_image ye_create_animation etc

input state machine

macro helpers for old style constructors

port interactions to ecs

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

## caching

ability to read the cache from a file into the cache in the background... could occur during splash screen and block until done... some sort of generic with loading screen also

## scene files

- the version integer denotes the version of the actual scene file format, not the version of the engine.
- any key not parsed by the engine can be used as a comment, like "comment": "this is a comment"
- if rect is unset it will be computed by bounds
- alignment integer is the cooresponding enum value.
  0 1 2
  3 4 5
  6 7 8
  with 9 being stretch to fill
- if flipped x or y are not present they are assumed to be false, really any sensible field that is not present is assumed to be a default value unless its critically necessary in which case it will just be skipped
- really any enum field in component struct will be a int in file

                    "rect":{
                        "x":0,
                        "y":0,
                        "w":0,
                        "h":0
                    },

it would be nice if scene manager knew entry point or last scene loaded to return to if critical error occurs

- prefabs array holds the path to any prefab files that should be loaded into the scene
  - if an item in the entities array has a prefab field, its current keys will be merged with the loaded prefab, any keys in the entity will override the prefab keys

## MEMORY LEAKS

- CRITICAL
  - all engine resource gets are STRDUPED?!??!?! WTF WAS I THINKING

## notes

- most things serialized to json are integers rather than floats.

## cache todo

The actual implementation for scene change persisting already cached textures

Caching prefab files assets pre. Loading prefabs into their own list so they are already constructed to instantiate? prefab is a field in entity struct but looks up into prefab list for it?

## camera

rendering at a scale and from a camera pov is necessary for an engine, but this is tilting me so unbelievably hard. figure it out or make it a limitation. bitch.

<https://chat.openai.com/share/8b657109-efd2-4b98-bf1c-0da309bed9f6>

## left off

editor test object into scene verify game resource path constructed from path (editor specific things change bc editor executible is completely different folder)

## renderer improvements

rectangle renderer for things like camera preview - maybe just internal implementation for editor?

## editor

have to think a lot about how we will represent the editor heiarchy and work with it. I think its best to allow editor to iterate over render lists by exposing them as pointers in some editor only datastructure - or in engine runtime state?
We will expose lists for editor to traverse on its own each frame and once we are satisfied with scene editor can dump it to a json (means we need to load all meta from json into editor for editing)
another problem to solve is that we also need to make font and color work seamless in the editor.

- figure out how we REALLY want to layout the editor. visual font and color loading would be nice but not strictly necessary for lack of time

- could technically have preview from other cameras POV shouldnt be too hard but honestly out of scope and unnecessary

- selected entity GOTO button

- nuklear deselect when lose focus (scrolling affects ui even if not mouse over)

- ui functions need decoupled from main, the setup of the editor project is really messy rn

- way to register C functions or files to attatch and run certain things on during lifecycle of engine

- better error detection and messages for paths rather than segfault. validate the result if the item even exists

- read "editor preferences" out of .yoyo for persisting some state between sessions. not rlly necessary but would be nice

## auto build vscode

tasks.json build on save

{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build Linux YoyoEngine on save",
            "type": "shell",
            "command": "${workspaceFolder}/engine/build_linux.sh", // Replace with the actual path to your script
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": [],
        }
    ]
}

## TODO

- finalize api for custom declarations of C scripts, and port editor over to use it as a genuine actual engine project

editor settings file, set nuklear ui color and font maybe

## Build system

- The custom libs linking have not been implemented into the build.py script

## nuklear

close button on window actually closes (no custom close buttons or keybinds)

- this would be nice for editor as well as console

```c
if(nk_window_is_closed(ctx, "Settings") == 1){
  project_settings_open = false;
  lock_viewport_interaction = false;
  remove_ui_component("project settings");
}
```
