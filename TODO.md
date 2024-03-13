# TODO

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

## considerations

a very soft rule is please no spaces in any file names (they will break in most cases)

we wont see colliders or paintbounds on cameras because thery dont get rendererd

## json best practice

- we currently mix fields of underscore and spaces, we need to pick one and rewrite all to be the same

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

combine multiple colors node editor
or use something for node editor

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

so I think the idea of the cache is alright, scenes are not massive so its ok to store literally anything that will ever be used in it. The thing is we dont get to intelligently destroy cache items becuase we have no idea how many things are actually using them. if you ever revisit the cache system you need to implement something like refcounting, which shouldnt be too bad since we have knowledge of the preprocessed scene file but can also add to the count in runtime if we get a cache hit

deltatime cooldown on editor inputs before updating them

TODO left;
cooldown so no spamming cache reload
same thing with images src

improve the console its lowkey ass. should be genuinely dynamic and auto select bottom input on open, and remember its last positions

maybe build and run should save first

## other QOL

like unity, if we are dragging a nuklear input please wrap the mouse around to the window border so we dont have to let go to redo

## existing known bugs

when renaming src input image dissapears without missing texture

god i hate the lines scaling thing

on x11 (wayland not tested or windows) sometimes fullscreen mode will literally disable your fucking monitor - maybe this stopped with last fix?

## rambling part ten billion

- memory leak when reload spamming over time
- right click popup menu to duplicate delete rename etc on entity buttons in list
- drop downs listing fonts and colors from the cache instead of typing in

## ecs

null checks for assigning and removing and blah blah fix all the ecs it doesnt have enough sanity checks - this is neccessary for the lua scripting system

## macro debugging

for all the stuff we only use debugging consider settings macros for it so its left out otherwise. also can cmake set macros? we could do that for platform dependance

## logging

rename logging symbols everywhere to be YE_LL_LEVEL

## lua scripting

read through references, you are doing bad practice for a lot of things like the naming convention

## doing the plugin system

how could we persist certain scripts or entities outside of refreshing when loading scenes? seperate list?

to call a lua function defined in a trick from lua scripts in engine it doesnt technically need to be registered yet because we can invoke it through a C helper function by name across lua states. but if we wanted to we could add a custom trick register lua callback

maybe in the future include src url in trick.yoyo which can auto update if you ask it to

something something inclues let other tricks build off of other tricks like modding sdks

## building

editor should not crutch on a build path existing, use its own

other:

progress bar would be cool, or a new console window with the build output

## editor

delete button push styles macro or function

## tricks plugin manager

add to meta last edited and then track so if user adds new item we auto refresh next launch instead of foccing them to click refresh

You need to majorly refactor this plugin manager. this is some of the worst code ive written in a long time. I need to stop programmign when I cant think straight.

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

## default tricks

there should be a popup in the launcher that lets you choose some default tricks to use, downloading them via submodule and adding them to the project

## new build system

go through and make sure you turn off unnecesary build targets like all the test and possibly SDL features that you arent using in the engine

## something

editor path is hard coded into settings now, rework this system

## misc tricks thoughts

ALSO TODO TOMORROW: editor build will freeze totally during the build. why?

TODO: fuck, i forgot to allow the game to link its own custom libraries. Force this to be through plugins?

## editor improvements

the editor should be a lot better about being project agnostic, so at any time we can open a new project, or maybe not. unity doesnt do that.

## todo tomorrow

games need to be able to link their own custom libaries, this might go hand in hand with the new trick stuff (tricks also need to be able to link their own deps).

this possibly takes the form of you exposing some cmake paths for tricks to manually do whatever with.

alt thought: tricks could technically be containers for libraries or dependancies, you would just need to port those deps to be inside a trick...

maybe you just construct a vendor folder and link anything in there with the game, while adding a new target before building the game to link against whatever deps..

this seems to lead to custom cmakelists.txt for the user to edit no matter what, not sure there is a way to do this all automatically

## trick auto updates

would be really hype to identify if we downloaded a trick from github and click check for update

## yep files

in the future we could hash the integer handles to get a unique id which would decrease file size slightly

a .yepignore would be nice to know which files we deliberately dont want to pack

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

## what if we exposed a really easy to use save data system?

- could just be a small wrapper on top of the existing json functionality
- this should come after 1.0, maybe even as an extension

## new platforms

- macos support seems to work out of box, need toolchain for osxcross

## misc x9999

- icon rework kinda goes hand in hand with engine resource stuff because by default rn we are using an icon in egnine resources

## youre gonna hate me for this

we should probably actually have compiler macros and shit to compile out the engine being in editor mode now that its add subdired
this wouldnt be too bad because its more just cutting out lines that we dont use and conditionally specifying paths and means of access, we can still leave the actual editor functionality in terms of visual debugging effects in the editor struct but turn it into like debugging field or something

## editor gdb

additional arg for gdb or something so that we instantly print out logs isntead of them not showing up

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

## new mixer

in terms of music tracks, we really only need one right now, but in the future we could expand the system to have a table of them for easy blending between different intensities and instruments

there are fading and other channel effects that could be fun to wrap around (and pretty easy too)

## qol editor

little widget icons for stuff like audio listeners and emitters

## misc 9123

I dont think every component has its relativity serialized rn

bumped nuklear ver for disabled feature, check what else has been added and if we want to use it. also implement the disabled on applicable places

check your style impl because with disabled widgets the checkbox does not fade color

maybe the issues with paintbounds is not casting to ints?

if renderer is not on/disabled we cant preview colliders or audio range

editor needs scene configureation/settings panel, add shortcut + menu bar + button in bottom maybe

prefabs would be nice but not sure how to best integrate that system

move over to build numbers for major? or semver... semver is hard to keep up with and I want to create major builds more often than build numbers would serve...

## mixer audio source

it might be smart to use the height as the falloff start range, because unity does that and its also smart to be able to controll where it sounds full volume and where it starts to fall off (this should not affect the mix positioning which is still relevant even if we are not falling off yet)

audio emitters are constantly playing, and just muted if outside range

BUG

- check memory leaks mix chunks

## otha

fading in and out scene loader transitions

scene settings panel editor

mixer start falloff range

audio listener specification rather than default camera center

## nuklear charts

```c
/* line chart */
            id = 0;
            index = -1;
            nk_layout_row_dynamic(ctx, 100, 1);
            if (nk_chart_begin(ctx, NK_CHART_LINES, 32, -1.0f, 1.0f)) {
                for (i = 0; i < 32; ++i) {
                    nk_flags res = nk_chart_push(ctx, (float)cos(id));
                    if (res & NK_CHART_HOVERING)
                        index = (int)i;
                    if (res & NK_CHART_CLICKED)
                        line_index = (int)i;
                    id += step;
                }
                nk_chart_end(ctx);
            }

            if (index != -1)
                nk_tooltipf(ctx, "Value: %.2f", (float)cos((float)index*step));
            if (line_index != -1) {
                nk_layout_row_dynamic(ctx, 20, 1);
                nk_labelf(ctx, NK_TEXT_LEFT, "Selected value: %.2f", (float)cos((float)index*step));
            }

```

## tmaps

### editor tmaps

honestly, lets just keep this basic tilemap impl for now, and in the future that could be a reworkable thing.
It involves more manual labor on the developer using the renderer but its really just typing in the pixels each time.

To make a true tilemap akin to unity it requires us to represent a ton of tiles in one renderer on one entitity potentially, unless we are doing like an entity painter thing,

It is technically possible to represent a grid logically in the editor, and make a brush tool to paint entities from presets, so maybe that could compound with this tilemap system

## LHJGFKJHDFKJGHKJDFGHJKFDHJG

audio chunk count removed or reworked

## slk fjlkgjh dfk gjhlk

collider add

trigger colliders

actual click selection and drag selection and other sensible controls, cant really click to cycle objects in order

should make a just normal run button for non rebuild conf changes

fade in / fade out would be extremely nice for scene transitions and splash screen

typing script path that doesnt exist creates it in editor

## future enhancements

- [ ] lua pre compile bytecode
- [ ] editor plugins (lua) which run in editor
- [ ] actual physics system with rigidbody simulation and solver
- [ ] gray out fields in editor when N/A
- [ ] fix the annoying renderer warp pixel bug, as well as the line scaling bug
- [ ] random scrollbars for panels that dont need it (horizontal mainly)
- [ ] asserts for all the things, better error handling (we dont want to be crash prone as I'm sure we are now)
- [ ] big ass refactoring and cleanup, especially of editor code
- [ ] dont rebuild clean object files
- [ ] camera zoom centered in center of viewport not screen

## things noticed from new project not streamlined

must open build settings to generate file or it doesnt exist just hitting build and run

should auto save or warn when building with no save

- for this would need to add a ton of lines everywhere we modify things that mark as dirty

DO NOT USE THE SCENE FILE CREATOR ITS TOTALLY BUGGED AND WILL WIPE YOUR SHIT

easy way for registering lua from C. like a fn quit built in

scene loading is cooked up somehow. trying to call through lua does not work because it attempts to call back into a nonexistant state (I think)

logging should be totally disabled to file unless we are in debug mode, also shouldnt log things we dont need to in release

add icons through cmake. make this go hand in hand with the current impl which is scuffy at best. we really only care about these icons on windows to lend credibility.

better error reporting for neccessary components - highlight the objects and components in red. ex: camera comps NEED transforms

## emscripten notes

Debugging web apps is a mixed bag. You should compile and link with -gsource-map, which embeds a ton of source-level debugging information into the build, and make sure the app source code is available on the web server, which is often a scary proposition for various reasons.

NO PLUGINS for emscripten (test later)

speed up builds with -j

force zlib and other examples to not build

## other build stuff

we have a release and debug mode, but im not totally convinced it works. at the least the sigs arent stripped because thats a linker option that we are throwwing to compiler. idk what im doing at all

-03 optimizes out the animation system for some reason lmao

## editor saving mark

it kinda makes no sense to show saving... if we are single threaded and blocking and dont update screen before

we dont track every single modification (properties which modify ecs directly) for unsaved dirty marking. how can we get around this without manually creating before and after copies for each field in each frame for comparison?

## misc lskgjhfljfd

clear console button

## discoveries during making game

default build settings

remove engine build path? - its used by build script for engine add subdir. think about how to make this better

tricks/ folder template not generated and if not present cannot build

heiarchy panel put things in a tab thing from overview nuklear so they can scroll inside and we have sticky buttons up top

editor keep last open scene saved when reopened

## screen

better caching and tracking of screen size and aspect ratios and other scaling letterboxing things so we dont recompute it in like ten places

get screen size uses the primary monitor

## notes

you are responsible for making sure a button youre polling for exists, because if youre checking for api state it will return false if it doesnt exist

fix the stretch alignment

## bugs with things

changing wrap length should visually show cutoff or update text TODO

## outlined text inspector

THERES NO OUTLINED TEXT IN INSPECTOR YET LMAOOO

## skjhfjkdfhgkjd

track editor camera position before scene reload and move it back to smae spot after recreated

multi select and duplicate and transform many at once

nuklear preview the .rc icon? could eventually be a cool addition
allow linking custom user libs without modifying build.py directly - maybe this ends up being some way to let them add cmake commands themselves

## audio shutdown and reinit is cooked

it calls the callback for each channel on shutdown which might actually cause rescheduling chunks after we shutdown

keepalive macro embedded in engine timer system

## skjhfgkjdfgkljflkjhdflkg hfdkljh

you made a txt file in kate with ideas
could do the unreal thing now. might not be worth the effort for drag tho

## cool others

darwin builds

emscripten wasm, is it possible to make savedata system store in browser?

issues with compiler optimization is probably casts. go through and fix them all.

## pre-acerola jam checklist

- I think rounding or cast errors with size_t or other are the cause of the compiler optimizations breaking animation system
- save build additional c flags per platform, so switching to windows will stash previous ones and restore them correctly when target changed
- maybe build.yoyo and settings.yoyo should merge into one settings.yoyo, feels weird that they are seperate

## remaining large missing features

- trigger colliders
- lua scripting API full

## other features from acerola jam that would be cool

- color tint controller
- ye_synchronize_position_relative called once on init and tracks one object onto another when they move during scene lifetime
- aseprite has a json export for sprite sheets, should add a way to import those automatically
- rounded colliders will be super important. probably also custom mesh colliders (could have made whole map out of one wall collider)
- utility function to pin entities onto others to track
- function to get transform locations but from the center of their renderer or collider or other utilities
- ye_debug_line to register at some point during frame before render to draw a line from two points
- allow setting rotation point through clicking world space in editor and converting that to relative to entity renderer
- easier timer constructor, they are more common than anticipated
- case insensitivity in search bar
- easily close hiearchy when searching to collapse it
- set the active entity to recently duplicated one
- enter key to submit imputs
- button to easily sync collider size to renderer size (rotation included)
- other collider shapes and skews
- expose renderer rect to manually cull instead of black boxes
- physics component duplification fields
- table of strings assosciated function pointers, use console commands to toggle engine debugging overlays

## bugs found during acerola jam

- physics system is fried with negative relativity for a collider, its offsetting the new position by the relative distance as well as the dx, dy
- physics system should treat each axis as a seperate calculation, so if youre hitting wall going north east, it should move you north if you are only colliding on the east axis
- copying an entity renames the OG to "copy" as well
- serialize or expose center of rotation in editor
- for missing assets, we used to have missing thing for images but yep will exit(1) if a header does not exist, so wrap this for images to ensure we get missing back if needed
- audiosource system is cooked beyond belief. trying to free memory that has alreayd been freed, or accessed idk its from setting volume of channels, check valgrind
- crashes on scene reloads a lot
- sync rotation on duplicated entities
- colliders are mega scuff, if more than one is touching things get weiiirdd
- adding intro seemed to cause inconsistant loading into entry scene

## things that might need done during acerola jam

- trigger colliders
  - trigger stay and trigger exit can come at a later date
