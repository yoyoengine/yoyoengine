# TODO

## custom symbols/icons

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

## confirmation pop ups notes

```c
  SDL_MessageBoxButtonData buttons[] = {
      { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" },
      { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "No" },
  };

  SDL_MessageBoxData messagebox;
  messagebox.flags = SDL_MESSAGEBOX_INFORMATION;
  messagebox.window = NULL; // Use NULL to create a simple message box
  messagebox.title = "Confirmation";
  messagebox.message = "Do you want to continue?";
  messagebox.numbuttons = SDL_arraysize(buttons);
  messagebox.buttons = buttons;

  int buttonid;
  if (SDL_ShowMessageBox(&messagebox, &buttonid) < 0) {
      SDL_Log("error displaying message box");
      return;
  }

  if(buttonid == 0){
      printf("yes\n");
  }
  else {
      printf("no\n");
  }
  exit(0);
```

## cut content

- it would be fun to make stuff like a file browser but at the end of the day im only one person and i would like to get a basic engine done, and then i can enhance it later on with fun stuff like that, it will be easier then as well ebcause my knowledge of nuklear will be better

need more robust way to know when errors happen in editor for things that might be hard or a lot of work to visually show- ie: scene file already exists but trying to create a new one of the same name

need some generic dialog popups as global state. functions can call like "input submitted" and get the result of the popup dialog if they were waiting for it, something like save or discard when loading a new scene

## Misc bugs

- we dont check if a tag already exists before adding currently

- when mouse enters viewport, deselect any nuklear windows open (heiarchy fixed location ones ofc)

- changing Z in editor only changes the value, it does not re sort the list.
  TODO: impl re sorting the list on z value change

- stretch alignment seems to default back to pixel size of image. might not be a bug, but actually a feature

- TODO: actual duplicate button in editor

- all #defines we should add a #ifndef before defining them so they can technically be set by user if they want to

- CCD substepping should be calculated based on the framecap

- create editor settings with defaults if it doesnt exist

- editor gotta get rid of bools tracking windows open, use the ui.c is exist function instead

- locking the viewport should be a refcount for how many windows are currently locking, that way windows can specify viewport should lock with them but if we open a ton of them at once there is no issue closing one, we still know to lock until all blocking are closed

- easily duplicate selected entity or delete it and other quick action shortcuts

- bug with changing scenes the title bar goes inactive until focusing another nuklear window then re attempting

## moving selected entity

CTRL clicking and dragging or shift clicking and dragging could move and scale the selected entity respectively without having to open the properties window

## thoughts

way to tottally save state of editor, so that we could have a play button that we click which just resumes running the scene at its current point as if it wasnt in editor- that way we dont have to build in order to quickly test like a physics collision

## premature optimization is the root of all evil

- who cares if we are reloading duplicate assets for the next scene, we could easily update this for fun later on, not force the optimization now when it doesnt matter. the engine is so lightweight anyways

## wrapped text

TTF_RenderText_Blended_Wrapped EXISTS HOLY SHIT

## considerations

a very soft rule is please no spaces in any file names (they will break in most cases)

we wont see colliders or paintbounds on cameras because thery dont get rendererd

## json best practice

- we currently mix fields of underscore and spaces, we need to pick one and rewrite all to be the same

## serialization

The font and color shit is rough, need to plan for serializing those.
Personally, I think its best to just use the cache system youve already created, you could maybe tweak for loading unique or global style files, but you should stick to the existing system (which really isnt bad) instead of rewriting it for the 100th time.

## Documentation

go back sometime and add < to every struct field so the docs include the field comment
use groups more

## color font stuff

in this case selecting fonts and colors could just be a dropdown in each editor menu possible

combo boxes reading from the persisted style tracking state... ?

## window activeness

we can actually change the active window inside nuklear.h:5674 with ctx->active.
The issue is that we dont have a reliable way to detect whether the user is doing something that we wouldnt want to defocus... other than locked viewport

## kalgdhljkdhglk

editor save prefs when closed, like the ratio and location of the editor window. also serialize all the overlays and options
better algorithm for selecting objects when clicking over them (unity has ruined me)
nuklear showing 1px margin scrollbars sometimes horizontally (depends on scaling)
reapproach editor scaling to be better to give higher size viewport eventually, also maybe repaint during rescale so its not so yuck
add existing project in launcher

## view ratios

to be honest, I am only partially the way there with the work im doing on the window ratio scaling and the letter and pillar boxing. in the future, we should be allowing IN EDITOR custom camera sizes and window sizes so that the camera size is automatically projected to a maintained window size and everything works with every valid value

## misc ten thousand

you should really remove the console logic from logging

when you do that you should overhaul it to be better and also interface with instead of toggle you can "set" a value to a input number or boolean

## nuklear shit

add horizontal tab thing to select which comp on entity, that way you can add new or delete existing

combine multiple colors node editor
or use something for node editor

seperators and tab stuff more for readability

## to fix hide mouse on drags

literally just patch the hiding mouse behavior out of nuklear you have access to source

## refacotr

editor_ui.c is a bloated mess - i moved a little bit of it just now but its not great. if you ever have to touch it again (you will) then refactor it

## misc 202304309505943

viewport seems to overlap title bar for bottom two editor panels

## panning zoom improvements

I would rlly like to have zoom center on camera center or mouse but idk the math for that
for panning I would like the pan to keep the mouse pointer on the world pos it started on

## cache size counter

i like the idea of this
also lua context size
also a console command to list cached textures by origin of type and name

## problems

in the editor we are spamming new text textures by creating one every time we type, and this is bloating the cache (i think)

TEXT IS UNIQUE IN ITS OWN RIGHT WHERE IT DOESNT NECCESSARIALLY NEED A CACHE FOR ITSELF, SO WE NEED TO BE MANAGING ITS MEMORY ISOLATED

so I think the idea of the cache is alright, scenes are not massive so its ok to store literally anything that will ever be used in it. The thing is we dont get to intelligently destroy cache items becuase we have no idea how many things are actually using them. if you ever revisit the cache system you need to implement something like refcounting, which shouldnt be too bad since we have knowledge of the preprocessed scene file but can also add to the count in runtime if we get a cache hit

enable terminal wrapping and resizing, why not?

the way this refactor is going you will have to register custom fonts programatically unless you use editor and colors alos

deltatime cooldown on editor inputs before updating them

TODO left;
cooldown so no spamming cache reload
same thing with images src

think about how to dynamically compute size?

improve the console its lowkey ass. should be genuinely dynamic and auto select bottom input on open, and remember its last positions

maybe build and run should save first

finish rest of renderers

## other QOL

like unity, if we are dragging a nuklear input please wrap the mouse around to the window border so we dont have to let go to redo

## engine features modules

discord RPC built into engine could be cool.
need some kind of module system to include optional dependancies.

## existing known bugs

when renaming src input image dissapears without missing texture

god i hate the lines scaling thing

on x11 (wayland not tested or windows) sometimes fullscreen mode will literally disable your fucking monitor - maybe this stopped with last fix?

## rambling part ten billion

- maintain heiarchy order when saving
- memory leak when reload spamming over time
- right click popup menu to duplicate delete rename etc on entity buttons in list
- drop downs listing fonts and colors from the cache instead of typing in

## emscripten build target

really only main loop needs changed, but you need to build all your deps as emscripten
<https://emscripten.org/docs/compiling/Building-Projects.html?highlight=sdl2>

build and run should save first

## ecs

null checks for assigning and removing and blah blah fix all the ecs it doesnt have enough sanity checks - this is neccessary for the lua scripting system

## macro debugging

for all the stuff we only use debugging consider settings macros for it so its left out otherwise. also can cmake set macros? we could do that for platform dependance

## logging

debug file doesnt seem to write to in builds

rename logging symbols everywhere to be YE_LL_LEVEL

## lua scripting

read through references, you are doing bad practice for a lot of things like the naming convention

whereever left off with lua scripting its pretty unhappy over some memory stuff, like the virtual address table handle lookups

## doing the plugin system

how could we persist certain scripts or entities outside of refreshing when loading scenes? seperate list?

to call a lua function defined in a trick from lua scripts in engine it doesnt technically need to be registered yet because we can invoke it through a C helper function by name across lua states. but if we wanted to we could add a custom trick register lua callback

maybe in the future include src url in trick.yoyo which can auto update if you ask it to

something something inclues let other tricks build off of other tricks like modding sdks

## plugin gui

plugin panel in editor that lets you enable or disable plugins, shows their meta, add plugin from local file, install plugin from url (git)

## building

editor should not crutch on a build path existing, use its own

other:

progress bar would be cool, or a new console window with the build output

## lua

memory issues so bad it page faults on windows and on linux it sometimes fails to munmapchunk but gracefully exits anyways

## editor

delete button push styles macro or function

## tricks plugin manager

add to meta last edited and then track so if user adds new item we auto refresh next launch instead of foccing them to click refresh

You need to majorly refactor this plugin manager. this is some of the worst code ive written in a long time. I need to stop programmign when I cant think straight.

## TODO NEXT TIME

literally the first thing on your list should be refactoring `editor_panel_tricks.c`, holy mother of bad code

## try to port over to better build system

list of all dependencies that need rebuilt:

- SDL2
  - do the mixer and other stuff get included?
- nuklear/uthash not applicable
- lua
- jansson
- all those mixed like libfreetype libmpg libpng

## cmake build api shit

cmake does not like to expose macros defined in source files to other source files, it has to be in headers. I wish I could just allow #define USE_PRE_LOOP to work but it seems like the user will have to go into a header file somewhere to define it no matter what if we really want it to work

```c
/*
    This file should have automatically been placed in your custom/include
    directory. This file contains all the C api definitions for the basic
    scripting callbacks and default game behavior.

    The intended use of this api is to make it easy to define custom C behavior,
    for example: if you wanted to load some custom data before the game initializes
    you could do the following in a new file like custom/src/mybehavior.c:
    ```c
    // include the api
    #include "yoyo_c_api.h"
    
    // tell the compiler you are using this function (the engine will recognize this)
    #define YOYO_PRE_INIT

    // implement the function as defined by its signature in the header
    void yoyo_pre_init(){
        // any behavior you want!!
    }
    ```
*/
```

## fix page faults in lua

lets just store as booleans whethere signatures exist and invoke them by name. no reason to extract their references in the tables out. we are just farming page faults for no reason

## default tricks

there should be a popup in the launcher that lets you choose some default tricks to use, downloading them via submodule and adding them to the project

## windows build error libpng

Im not sure what this is, I think its that it doesnt actually say that it links and requires the other libs in the windows dissasembler

## cmake makes me want to end my life

progress so far with fetch content:

```cmake
cmake_minimum_required(VERSION 3.22.1)
project(yoyoengine)

file(GLOB SOURCES src/*.c CMAKE_CONFIGURE_DEPENDS)

add_executable(yoyoengine ${SOURCES})

include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)

###############
#   jansson   #
###############

set(BUILD_SHARED 1 CACHE INTERNAL "")
set(JANSSON_BUILD_DOCS OFF CACHE INTERNAL "")
FetchContent_Declare(
    jansson
    GIT_REPOSITORY https://github.com/akheron/jansson.git
    GIT_TAG        v2.14
    GIT_PROGRESS TRUE
    # CMAKE_ARGS     -DBUILD_SHARED=1 -DJANSSON_BUILD_DOCS=OFF  # Build jansson as a shared library
)

FetchContent_MakeAvailable(jansson)

target_include_directories(yoyoengine PRIVATE ${jansson_SOURCE_DIR}/include)

... truncated ...


target_link_libraries(yoyoengine PRIVATE jansson SDL2 SDL2_image SDL2_mixer lua_static)
```

## new build system

go through and make sure you turn off unnecesary build targets like all the test and possibly SDL features that you arent using in the engine

## something

editor path is hard coded into settings now, rework this system

## cmake gripes

- fix so that it doesnt rebuild the entire thing EVERY TIME

NEXT STEPS:
finish for the normal project build system that is autogenerated
add for tricks, just make them use a cmakelists.txt and they can fetch content their own deps and statically link them.

## misc tricks thoughts

ALSO TODO TOMORROW: editor build will freeze totally during the build. why?

TODO: fuck, i forgot to allow the game to link its own custom libraries. Force this to be through plugins?

## editor improvements

the editor should be a lot better about being project agnostic, so at any time we can open a new project, or maybe not. unity doesnt do that.

## tricks thoughts ten billion

maybe there is an issue with tricks callbacks running during startup? probably not

## todo tomorrow

games need to be able to link their own custom libaries, this might go hand in hand with the new trick stuff (tricks also need to be able to link their own deps).

this possibly takes the form of you exposing some cmake paths for tricks to manually do whatever with.

alt thought: tricks could technically be containers for libraries or dependancies, you would just need to port those deps to be inside a trick...

maybe you just construct a vendor folder and link anything in there with the game, while adding a new target before building the game to link against whatever deps..

this seems to lead to custom cmakelists.txt for the user to edit no matter what, not sure there is a way to do this all automatically

## trick auto updates

would be really hype to identify if we downloaded a trick from github and click check for update

## windows builds

we want to be able to run it without console opening

we want to link an icon to the exe

## yep files

in the future we could hash the integer handles to get a unique id which would decrease file size slightly

a .yepignore would be nice to know which files we deliberately dont want to pack

### todo when you read this (i actually left some fun stuff this time)

- start case switching what file type we are looking at in the packer
- load images (can only be bmp jpeg jpg png) into RGBA arrays (research this with SDL)
- same with PCM and audio (mp3 and wav i think only)
- research online doing this with ttf files? I dont think we can do atlas because its rebuilt a lot (unless we build an atlas for every single size game uses at runtime which we couldnt ever know because of dynamic scripting)
- draft api for engine reading this data out of the yep file
- force the build system to not copy resources directories but the yep instead
- hook up every location where we load resources to instead load from the yep file
- refactor get resource function to just be like a get path
  - this involves removing references to this that dip into /resources/.. to get somewhere (despair)

## backup of old image loading system

```c
SDL_Surface *image = IMG_Load(itr->fullpath);
            if(!image){
                ye_logf(error,"Error loading image %s\n", itr->fullpath);
                exit(1);
            }

            // print out the format of the image from the surface
            SDL_PixelFormat *format = image->format;
            
            // init some vars that are specific to the format we work with
            uint8_t bytes_per_pixel;

            // switch on the format type to handle different formats
            switch(format->format){
                case SDL_PIXELFORMAT_RGB24:
                    ye_logf(debug,"Image format: RGB24\n");
                    data_type = (uint8_t)YEP_DATATYPE_IMAGE_RGB24;
                    bytes_per_pixel = 3;
                    break;
                case SDL_PIXELFORMAT_ABGR8888:
                    ye_logf(debug,"Image format: ARGB8888\n");
                    data_type = (uint8_t)YEP_DATATYPE_IMAGE_ABGR888;
                    bytes_per_pixel = 4;
                    break;
                default:
                    ye_logf(debug,"Image format: UNKNOWN\n");
                    exit(1);
                    break;
            }

            printf("Image format: %s\n", SDL_GetPixelFormatName(format->format));

            // check if this requires locking (VERY UNLIKELY)
            if(SDL_MUSTLOCK(image)){
                SDL_LockSurface(image);
                printf("Locked surface\n");
            }

            // alloc the data and copy into it
            int malloc_data_size = sizeof(uint16_t) + sizeof(uint16_t) + (image->w * image->h * bytes_per_pixel);
            data = (char*)malloc(malloc_data_size);

            // check for if width exceeds the max value
            if(image->w > UINT16_MAX || image->h > UINT16_MAX){
                ye_logf(error,"Error: image %s width exceeds max value of %d\n", itr->fullpath, UINT16_MAX);
                exit(1);
            }

            uint16_t width = image->w;
            uint16_t height = image->h;
            memcpy(data, &width, sizeof(uint16_t));
            memcpy(data + sizeof(uint16_t), &height, sizeof(uint16_t));

            // need to dereference the void pointer for pixels
            uint32_t* pixels = (uint32_t*)image->pixels;

            memcpy(
                data + sizeof(uint16_t) + sizeof(uint16_t), // offset from the w,h meta
                pixels,                                     // pixel data
                image->w * image->h * bytes_per_pixel       // size
            );
            
            printf("Image %s is %d x %d\n", itr->fullpath, image->w, image->h);

            // set vars
            data_size = sizeof(uint16_t) + sizeof(uint16_t) + (image->w * image->h * bytes_per_pixel);
            uncompressed_size = data_size;
            compression_type = (uint8_t)YEP_COMPRESSION_NONE;

            // cleanup
            SDL_FreeSurface(image);
```

## new systems that need done

### audio rewrite

- spatial audio with emitters and listener
- one track for music that is streamed in

### animation / tilesheet

- animations need batched into one single file, we modulate the src rect to get frames
- similarly, a tilemap system is needed to easily create levels
- we need to be able to easily slice and access parts of tilemaps and texture atlases
- could be a simple solution to create an importer for altases created outside like in aseprite or other software and read them that way rather than pack myself

## what if we exposed a really easy to use save data system?

- could just be a small wrapper on top of the existing json functionality
- this should come after 1.0, maybe even as an extension

## new platforms

- emscripten doable with a toolchain file
- macos support seems to work out of box, need toolchain for osxcross

## misc x9999

- icon rework kinda goes hand in hand with engine resource stuff because by default rn we are using an icon in egnine resources

## key to uploading sdl images to nuklear?

```c
NK_INTERN void
nk_sdl_device_upload_atlas(const void *image, int width, int height)
{
    struct nk_sdl_device *dev = &sdl.ogl;

    SDL_Texture *g_SDLFontTexture = SDL_CreateTexture(sdl.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);
    if (g_SDLFontTexture == NULL) {
        SDL_Log("error creating texture");
        return;
    }
    SDL_UpdateTexture(g_SDLFontTexture, NULL, image, 4 * width);
    SDL_SetTextureBlendMode(g_SDLFontTexture, SDL_BLENDMODE_BLEND);
    dev->font_tex = g_SDLFontTexture;
}
```

## youre gonna hate me for this

we should probably actually have compiler macros and shit to compile out the engine being in editor mode now that its add subdired
this wouldnt be too bad because its more just cutting out lines that we dont use and conditionally specifying paths and means of access, we can still leave the actual editor functionality in terms of visual debugging effects in the editor struct but turn it into like debugging field or something

## editor gdb

additional arg for gdb or something so that we instantly print out logs isntead of them not showing up

## windows icon

its pretty easy to setup icons but should do it in a platform agnostic way, cant always pack into file because might need loose, cmake takes an arg

## disabled things

engine startup noise is disabled until audio rework - actually all audio is

for some reason only the game launched by the editor will fail to find the yep files. otherwise its fine when launched from its own build script
^^ i would fix this now (sorry for leaving future me to do it) but this is such a mega commit that I think youll forgive me

## build improvements

different build/release modes

## editor improvements x2

the layout and panels could be cleaned up

would be nice to have overlays and preferences (paintbounds) sync to settings

custom borderless layout like UE5

stretch viewport gone

condense options

actually be able to drag and resize things

run the game without building (should be possible)

(way mega stretch) lua editor inside the editor

save confirmations and popups are really important

build currently blocks while executing:

```c
/*
    Investigation into fixing blocking builds:
    - pthreads
    - pipes to communicate with build script
*/
```
