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

- bug in nuklear? sometimes it hides when proprty dragging sometimes it doesnt <https://github.com/vurtun/nuklear/issues/631>

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

it honestly might be better if colors werent in this weird ahh cache system, but this is how ive set it up so i might just double down

it would be better if we had a font manager that tracked the name and path to the ttf, as well as all created sizes of the font, that way we dont have to create a new font key for each size

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

## SQUASH

consider going back and squashing all those doxygen commits, there are a shit ton of them

## color font stuff

should keep existing cache system.
in this case selecting fonts and colors could just be a dropdown in each editor menu possible, with a popout that allows modifying the style.yoyo. Ideally though for performance we wouldnt want to load everything from a global style.yoyo but tbh this is good enough just to have a usable system, and the code is generic enough to be refactored to read and load fonts and colors from any source later on.

- to take in names we need to refactor the storage of colors as the font pointer or name so we can reverse engineer what color it is - maybe just attatch its cache node to the struct

## ideas

combo boxes reading from the persisted style tracking state... ?

## window activeness

we can actually change the active window inside nuklear.h:5674 with ctx->active.
The issue is that we dont have a reliable way to detect whether the user is doing something that we wouldnt want to defocus... other than locked viewport

## window resizing

for the editor:
resizing the window will change the editors known window size, this size is used for the editor to determine the bounds of the viewport. the size of the heiarchy and other windows are fixed and the viewport is all the space thats not taken up by this.

im going to commit what I have now, but for the clamping resizing I am basically thinking that we still scale the nuklear sizes based on the window size, but they can only grow so large or small, that way we preseve visibility.

alt approach, maybe we dont need to clamp at all but we can just keep constant size like 500px for side panels, (does not let us work at lower res) if we do that probably include a float scaling factor to allow user to customize size of ui scaling

## build and other icons
I can also just straight up write in the nuklear.h my own icon definitions (hard)


sdl renderer backend how are images formatted and stored?? holy fuck how can i upload an image to nuklear
what format is nuklear expecting??

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
