# Refined TODO

## Build 0 Essential

Nothing!!!

## Build 1+

### Refactoring

- Refactor the FUCK out of the editor. It has the stinkiest code ever
- Refactor and rework the plugin manager
- How can we simplify serialization and boilerplate for each component
- rework how the ui system works. its pretty jank that we register window cbs, i think nuklear has a built in way of handling this
  - you could look at how that CAD guy uses it

### Bugs

- Changing an entity's Z value in the editor does not currently re-sort the renderer entity list
- Replace editor booleans with is_exists checks
- Sometimes nuklear panels (title bar) go inactive when doing things like loading new scenes in editor
- There might be a lot of places that break with spaces in filenames
- nuklear showing 1px margin scrollbars sometimes horizontally (depends on scaling)
- There is not enough sanity checking (null checks and validity checks) in the ecs system
- Get screen size uses only the primary monitor (sdl bug?)
- Multiple colliders touching gets weird
- valgrind runs weirdly for editor, serializing editor components and segaulting and gp faulting
- Scrollbar resets when selecting entity in heiarchy
- shift ctrl select entity to do range in heiarchy?
- When opening to welcome page, fullscreen the window and then load into a project. you will notice a weird viewport bug that is fixed by resizing the window.
- when zenity prompts, we minimze and then maximize which destroys the active reference in nuklear (even though the sdl backend does not touch this)
- console underneath group panels on welcome page

### Features

- Template should have C++ option, and integrate that into editor when creating project
- Easy constructor for debug panels, particularly a datatype and interface functions for debug charts (like in roguefight)
- Viewport jank
  - Scrolling on panels should not scroll the viewport
  - zenity refocus causes us to need to click a second time (might be a DE thing)
  - we can actually change the active window inside nuklear.h:5674 with ctx->active.
- Keyboard shortcut pass to do things like easily duplicate or delete
- It would be nice to be able to move entities around and scale, rotate them freely by holding a certain key and dragging. Like if you click on them and start dragging.
- Model tracking fonts and colors, so we can use a dropdown rather than typing in their key names
  - Can have a default so there is no faulty lookup (engine_default)
- Custom camera sizes, so the letterboxing is not hard coded to 16:9, would be awesome to see a yoyoengine game running in 21:9 or mixed aspect ratios
- Debug panel count the size of the cache and of lua states
  - maybe a console command to print out the cached resources as a list
- When dragging nuklear input and mouse leaves the window, wrap to the other side? would this mess with the nuklear handling?
- Right click on entity list entries to get context menu to do useful things like delete and duplicate? In the future nesting options?
- Plugins are auto loaded from dir, not linked against. That way modders can build plugins that just need to be put in the folder
- Yep rework
  - Move code out into a seperate repo
  - yep explorer application
  - allow engine to read any arbitrary yep files
    - games could pack everything for a scene into a yep
- Save data abstraction system (especially useful to lua)
- Mac and windows support for editor
  - grind tf out of some platform specific macros
- Prefabs
- Be able to specify audio listener (component?) rather than camera center (maybe just let it be a point in space)
- Actual tilemap editor and support (grid placements).
  - Tilemap would be its own renderer, with a grid of integers to represent tile indexes, engine is optimized to render online seen tiles
- Fade in/out scene transitions
- Wrapping text should show the cutoff distance and visually wrap in editor
- Color tint controller inside renderer (note: we have to tint and untint because this texture is cached between many entities)
- Aseprite .ase support
- Allow setting rotation point through editor
- Case insensitive search in editor
- Ontrigger stay ontrigger exit
- lua debug renderer wrapper
- Per entity blend mode exposed
- Search overhaul
  - Filter by entities that have components?
- lua entity should have a reference to self, since it wont change over lifetime
- Copy paste components into other entities
- Nuklear has tips on font rasterization, we could tweak to make it look better
- Toasts or some system to show information and inform of erros rather than just the console
- How awesome would it be if you could pop open a debug inspector at runtime of any game...
- Nuklear docking fork
- In console command system: tab for completing or suggesting commands, implement -2 flag for command to provide suggestions itself
- Editor mode flag should become relevant to the engine. it would be nice if we could have a lightweight editor inside of shipped games to debug (as a module so we can disable it when prod). inspired by visu
- We need to get closer to nuklear native window management, so we can spawn preset tool windows. for example: windows like generate hitbox masks or windows to preview asset packs

### Misc

- Lock viewport should be a refcounted system
- Actually commit to doxygen
- Refactor logging
  - Name the levels better
  - Move the ui handling out of logging lol, a ton of places import logging just for nuklear
- Editor build progress bar
- Little widgets to show autio listeners and emitters
- Clear console button
- Some kind of caching for fetchcontent would be awesome
- Nuklear has find_window calls, we shouldnt have our own weird ass abstraction on top
- Resizing window spams terminal
- empty color or font change buffers spam errors
- option to auto focus terminal when error shows in editor
- RPATH/LIBC:
  - because we set rpath to be relative to origin, somehow this means we need a libc that is as new as what we built on (which for some reason is not the case without setting the rpath). So right now distros with an older libc compared to the build machine cannot run it
- It would be awesome to have a suite of debug panels which can be enabled through the console at any time, rather than setting config bools (just expose them to the console?)

### Far out / Just shaping

- if we could totally save the editor and ecs state, we could enable game mode in the viewport and let it play out, but then hop back into editor mode
- Check for duplicate handles in next scene we are about to load and preserve them if they are already loaded into cache
- Maybe expose the engine state as modifyable in a panel to nuklear
- nuklear node editor for animation blending?
- write a test suite or almost fuzzer type game to add components to entities in specific orders to try to catch npd
- A way to persist lua states or specific entities between scenes?
  - Persistant entity list only created and cleared on startup and shutdown?
- Custom borderless layout like UE5
- Actually be able to drag and resize things
- Real entity nesting or multicomp (probably infeasible)
- Some meta music system that allows fading and mixing between tracks of different intensities (this is probably better suited for game devs to implement themselves)
- Fork nuklear and maintain it outside of the engine repo
- Pre compiling lua bytecode
- Should we auto save or warn when building without saving? make this a pref?
- Can we leverage SDL file drop event?
- lua API fuzzing or unit tests
- keyframed animation system for like cutscenes
- movie player component renderer
- vram usage in debug panel?
- Give user some form of customization for the layout of editor
- let users make their own color themes, expose color selectors in editor
  - tint icons for light dark themes
- Pipe between running game and editor to live debug and monitor
- launcher: there should be local update option rather than installer from github so you can dist prereleases

### wishlist

- hot reload
- crash reporter

## Nuklear fixes / enhancements (give back)

- label icon buttons which have flag to collapse back into just icons depending on resizing
- weird 0px scrollbar bug

## TODO NOW

- engine splash screen is dated asf

## categorize later

components relative by default

components have aspect ratio lock sensibly (button)

## WIP console rewrite

- give default admin commands
  - go deep with it, let us modify the engine runtime state

## TAR PHYSICS

- collision detection
- solver
- LUA API's for colliders and rigidbodies
- triple check accounting for rotation in all the other systems... should button, audiosource, be effected? should relative spin around the center of the transform??

## lskdjgfl kjdfjug klfdhgkldfhjg

- pass on each component for linalg (if needed)
  - needed for button click detection rn

## bruh refactors

SDL_Image should be wrapped by ye_image which contain meta on size... we need this for stupid edge stuff like tilemap renderer

## ldkjhfgkjdfhgkjdfhgd

- god, stupid annoying yep invalidation... you need to just solve this one and for all
  - idek, do like yep as a cli tool or something... then you ship the binary with the engine but that makes it so complicated. maybe just make it a library and link it with both the editor and engine? idek just think about it man

## random way out there ideas

it would be cool to have a nuklear inspector panel at runtime, to debug entities, also to freeze time and do other things, that debug menu could be expandable maybe

## next up

- bust out some of the renderer transformations into helper functions and re-use them to implement for buttons & camviews & audiosource etc

## todo

- click to cycle through by z order?

debug renderer not respecting vert alphas (me, im the alpha!!!)

UNKNOWNS

- should entities that ARE NOT relative have their own rotation or still inherit from parent?

skjgdflksdjfgs

sometime in the future we should make it so clicking on overstacked ents will cycle through them by z or other order

LAST FINAL BEFORE TAR:

- etc

camera rotation??

## sdklfukjdfhgkdfjhgdjkfhg

ok you have some braindead design decisions where the editor is the one that builds the engine pack, so im thinking you need a build toolchain involving the yep that is NOT embedded in the editor.

various paint overlays dont show when renderer not visible or being painted...

## ideas

expose in config a system panel which visualizes engine config settings at runtime

when we editor select based on a comp, set that comp as the active comp in the inspector

be suuuper clear about build platforms in editor dropdown... only enable installs if you have the toolchains or opt in idk

## yep2

creates datastructure tree with stamps for each file, selectively repacks only changed files... something like that to rapidly accelerate builds

can also be used to have a header hash map structure open at runtime for each pack, for quick lookups.
We can also probably just open each pack file since it doesnt require mapping every single one into memory fully. faster than open/close switching

## lua binding slop

- rigidbody
- new renderer changes (center)

## misc

scene reload crash

future:
- ui.c could be used to create a nuklear docking generic solution...

## way distant future

localization

improved theme support: tag themes as "light" or "dark" and tint the icons before uploading to gpu

extend all the custom yoyoengine colors to actually use the nuklear definitions for cohesion

move theme support into the engine level, so we can theme the games too

iron out amoled and merge into nuklear

## slkdjglsikdjf

break up engine.c into multiple files in core/
start shifting more globals into ye_state

nk_window_is_any_hovered can tell when exiting viewport

you need to just scroll through nuklear.h. badly

## lksj flksdg

im not so sure the camera plays nice with rotation lol

scene camera viewport toggle useless
remove stretch viewport?

## future commands/improvements

- overlay auto generated panel to nuklear checkbox toggle
- info command with build details and credits for runtime core

## crossplat!!!

- valgrind it (bunch of errors)
- turn on every sanitizer for dev builds

## general improvements

- refactor welcome panel networking hit to be in background (panel loading symbol)
- refactor in general to not be gross
- curl wrapper instead of hard coding

## cmake improvements

- sdl3 seems to build zlib itself- try to make it so this only happens once
- nuke all of the vendored deps which you dont use that the satellite libs are pulling in for file types
- Lilith might be duplicating in P2D, maybe you can just do something like the satellites where you can specify it already exists to propogate

## TTODO

- Go add file picker to all the places its needed (scene open)
- Use SDL3 for all the file access thats not cross platform
- TODO NOTCROSSPLATFORM

## maybe

- could rewrite some of the editor picker callbacks to just be normal ptr writes, they already have validation in the main func
- move file picker capabilities into the engine, so it can be extended whenever

## brain

- color and font just be drop down

## Roadmap

- Replace NOTCROSSPLATFORM
  - Threading builds
  - Yep

## skldjgskfdjhkdfjghkdfjhg

- Decide on pattern for threading/pipeing from running games in editor
- yep platform stuff (replace iteration)


## long future

- rework plugin system
- rework scripting
  - add references in scripts

## lkdjfglkdfjg

- probably need a is_resources_changed recursion that checks every file or something.
  - current check doesnt care if we modify a existing file in resources for some reason (i think, you should check)

## WINDOWS PORT

- lla_vec_magnitude sigsegvs

## QOL

- add SDL_GetError wrappers literally everywhere in logs
