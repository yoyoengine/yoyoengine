# engine TODO

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

## notes

- most things serialized to json are integers rather than floats.

## cache todo

The actual implementation for scene change persisting already cached textures

Caching prefab files assets pre. Loading prefabs into their own list so they are already constructed to instantiate? prefab is a field in entity struct but looks up into prefab list for it?

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

- | NK_WINDOW_BACKGROUND with initializing heiarchy and other perma-background windows will keep popups from getting lost in them, but at the tradeoff of seeming disabling scroll-over behavior

- frame glitch when deselecting entity because we break before the frame can finish painting it NOT there (i think)

- logical groups in scene files (does not actually pair items together, or does it?). lets you organize things together to collapse them in hiearchy

- editor settings would be nice, change gui theme and others... layouting?

- editor needs the ability to accomidate differently sized cameras... it would ceratinly be ideal if we could paint the size of the viewport that has no ui covering it so we can manipulate more of the scene at once, like unity does

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

## TODO TOMORROW

- I mentally wrestled with memory leaks over using internal jansson memory vs mallocing in the beginning, but that was overwritten by some fields in jansson,
  - TLDR: currently we rely on the json for settings and build already having every key existant or it fails, to counteract this we could malloc up front and then strcpy the jansson temp values into the final global holders.
  - decide if this is worth it or not or make a note to fix this limitation later on.
- project settings, set icon path

## tags

- we dont check if a tag already exists before adding currently

## editor bugfix

- when the game is running the editor is unresponsive and not re-rendering

- deleting scene camera crashes

## decide on relative and offsets

- what components should even be able to have positions? everything should just inherit from entities transform?

## colliders

- collider and paintbounds arent painted unless entity has a renderer

- how exactly do colliders work... static should not be able to move through static, trigger will trigger from anything that collides with it

## benchmark timers

```c
  ye_register_benchmark_timer("engine frame stall",&output);
  ye_start_btimer("engine frame stall");
  ...
  ye_stop_btimer("engine frame stall");

  printf("engine frame stall: %f\n",output);
```

## bugs and ignoring stuff

- physics system currently dgaf about the entity bounds transform, maybe it should only modulate bounds and then the renderer can auto calculate the rect
- dumb chatgpt collision hack
- temp input handling for projects, but really should be default so you can close it without having to write that yourself
- depending on framerate things will collide at different places:
  - check if we totally went through another collidable object and if so, move back to the last position
  - if we are going to collide, after setting velocity to nothing move to the furthest possible position without colliding

## custom symbols

NK_LIB void
nk_draw_symbol(struct nk_command_buffer *out, enum nk_symbol_type type,
    struct nk_rect content, struct nk_color background, struct nk_color foreground,
    float border_width, const struct nk_user_font*font)

is what you would modify

saving scene, editor mouse invisible bug, edit styles and others, open files in vscode.

- bug in nuklear? sometimes it hides when proprty dragging sometimes it doesnt <https://github.com/vurtun/nuklear/issues/631>

- solve world mouse point in editor

resources i found on custom symbols while i give up for now:

```c
  struct nk_image {nk_handle handle; nk_ushort w, h; nk_ushort region[4];};

  nk_image_ptr(void *ptr)
  {
      struct nk_image s;
      nk_zero(&s, sizeof(s));
      NK_ASSERT(ptr);
      s.handle.ptr = ptr;
      s.w = 0; s.h = 0;
      s.region[0] = 0;
      s.region[1] = 0;
      s.region[2] = 0;
      s.region[3] = 0;
      return s;
  }

  SDL_Surface *IMG_Load(const char *file)
  {
  #if __EMSCRIPTEN__
      int w, h;
      char *data;
      SDL_Surface *surf;

      data = emscripten_get_preloaded_image_data(file, &w, &h);
      if (data != NULL) {
          surf = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_ABGR8888);
          if (surf != NULL) {
              SDL_memcpy(surf->pixels, data, w * h * 4);
          }
          free(data); /* This should NOT be SDL_free() */
          return surf;
      }
  #endif

      SDL_RWops *src = SDL_RWFromFile(file, "rb");
      const char *ext = SDL_strrchr(file, '.');
      if (ext) {
          ext++;
      }
      if (!src) {
          /* The error message has been set in SDL_RWFromFile */
          return NULL;
      }
      return IMG_LoadTyped_RW(src, SDL_TRUE, ext);
  }

  //BROKEN CHATGPT SNIP
  // static struct nk_image icon_load(const char *filename, SDL_Renderer* renderer) {
  //     SDL_Texture *texture = NULL;
  //     SDL_Surface* surface = IMG_Load(filename);
  //     if (!surface) {
  //         // Handle loading error
  //         printf("[SDL]: failed to load image: %s", filename);
  //     }

  //     // Create a texture from the loaded surface
  //     texture = SDL_CreateTextureFromSurface(renderer, surface);
  //     SDL_FreeSurface(surface);  // Free the surface since we no longer need it

  //     struct nk_image nuklearImage;
  //     nuklearImage.handle.ptr = texture;  // Use the texture as the handle
  //     nuklearImage.w = 150;
  //     nuklearImage.h = 150;

  //     return nuklearImage;
  // }
```

<https://github.com/vurtun/nuklear/issues/476>
<https://www.reddit.com/r/opengl/comments/ej43ct/help_with_nuklear_how_to_display_an_image_buffer/>
<https://www.khronos.org/opengl/wiki/Image_Load_Store>
<https://github.com/libsdl-org/SDL_image/blob/0eddb391e57a6b11a50dfdb61986315030480985/src/IMG.c#L277>
<https://github.com/libsdl-org/SDL_image/blob/0eddb391e57a6b11a50dfdb61986315030480985/src/IMG.c#L217>
<https://wiki.libsdl.org/SDL2/SDL_RWops>
<https://gamedev.stackexchange.com/questions/131138/how-to-pass-png-image-data-directly-to-sdl>
<https://github.com/vurtun/nuklear/blob/master/example/file_browser.c#L539>
<https://www.reddit.com/r/opengl/comments/ehkmvv/help_displaying_image_buffer_in_nuklear/>
<https://github.com/Immediate-Mode-UI/Nuklear/blob/master/demo/common/overview.c#L2>

## editor polish

- when mouse enters viewport, deselect any nuklear windows open (heiarchy fixed location ones ofc)

each component should have its own z

TODO: it would be nice if adding a renderer auto added it as the size of the image

changing Z in editor only changes the value, it does not re sort the list.
TODO: impl re sorting the list on z value change

stretch alignment seems to default back to pixel size of image. might not be a bug, but actually a feature

BUG: mouse world point is incorrect if cam is scaled and we are stretched viewport.

actual editor needs state struct. its getting hard to track everything

TODO: actual duplicate button in editor

## physics

- we still need to move on last frame before full collision to max distance possible without colliding
- we also on lower framerates phase through walls entirely because next position is outside them. we could take the staged position and just create a path rect from initial to staged and check that for collision because its any place between it would have been

## mental notes

- things that need to happen
  - adding components to entities
  - duplication fixes and button in editor
  - serialize/update the scene file with new data from scene
  - onclick interaction handler, message system
  - prefabs and styles editing

## styles baked in on a per scene basis and can reference an outside sheet too? easy way to edit them in editor without openign files? maybe thats for later

## cut content

- it would be fun to make stuff like a file browser but at the end of the day im only one person and i would like to get a basic engine done, and then i can enhance it later on with fun stuff like that, it will be easier then as well ebcause my knowledge of nuklear will be better

need more robust way to know when errors happen in editor for things that might be hard or a lot of work to visually show- ie: scene file already exists but trying to create a new one of the same name

need some generic dialog popups as global state. functions can call like "input submitted" and get the result of the popup dialog if they were waiting for it, something like save or discard when loading a new scene

## tilemaps

having good first class tilemap support would be huge

## debug frame stepping

allow a crazy low timestep debug view type thing so we only advance frames when told, even though we keep rendering them we dont paint anything new or refresh, just keep presenting the same buffer

will be super useful for physics

all #defines we should add a #ifndef before defining them so they can technically be set by user if they want to
