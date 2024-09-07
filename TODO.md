# Refined TODO

## Build 0 Essential

Nothing!!!

## Build 1+

### Refactoring

- Refactor the FUCK out of the editor. It has the stinkiest code ever
- Refactor and rework the plugin manager
- How can we simplify serialization and boilerplate for each component

### Bugs

- Changing an entity's Z value in the editor does not currently re-sort the renderer entity list
- Replace editor booleans with is_exists checks
- Sometimes nuklear panels (title bar) go inactive when doing things like loading new scenes in editor
- There might be a lot of places that break with spaces in filenames
- nuklear showing 1px margin scrollbars sometimes horizontally (depends on scaling)
- There is not enough sanity checking (null checks and validity checks) in the ecs system
- Get screen size uses only the primary monitor (sdl bug?)
- Physics system MIGHT be broken with negative relative colliders
- Physics system should compute axies seperately
- Multiple colliders touching gets weird
- valgrind runs weirdly for editor, serializing editor components and segaulting and gp faulting
- Scrollbar resets when selecting entity in heiarchy
- shift ctrl select entity to do range in heiarchy?
- When opening to welcome page, fullscreen the window and then load into a project. you will notice a weird viewport bug that is fixed by resizing the window.
- when zenity prompts, we minimze and then maximize which destroys the active reference in nuklear (even though the sdl backend does not touch this)
- console underneath group panels on welcome page

### Features

- Viewport jank
  - Scrolling on panels should not scroll the viewport
  - zenity refocus causes us to need to click a second time (might be a DE thing)
  - we can actually change the active window inside nuklear.h:5674 with ctx->active.
- Physics system rewrite
  - Seperate simulation step parameters
  - Actual physics solver with rigidbodies and gravity, velocity, drag, etc
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

## Nuklear fixes / enhancements (give back)

- label icon buttons which have flag to collapse back into just icons depending on resizing
- weird 0px scrollbar bug

## TODO NOW

- engine splash screen is dated asf
