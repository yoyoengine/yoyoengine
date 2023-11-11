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

would be cool in editor somewhere - top bar? to have a X warnings, X errors yellow and red respectively to draw attention

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
