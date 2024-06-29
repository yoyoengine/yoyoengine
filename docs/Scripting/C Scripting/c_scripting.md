---
tags:
  - c
---

# C scripting

Since I have transitioned this project to be more of an engine and less of a framework, by using the launcher to create a new project you will recieve a structure that is preconfigured to build and run a game by default.
In order to define custom behavior (ie: actually script your game) you should make note of the `custom/` directory created in your project folder.

```txt
custom
├── include
│   └── yoyo_c_api.h
├── lib
└── src
```

This directory is where all of your code for the game should go. You will notice that its only populated with one file by default, which we will touch on in a minute.

## To link libraries

As of writing this, I am working on rewriting build scripts. This is subject to change and will be updated soon.

## Headers

`include` is provided for your convenience, and is added to the include path automatically. You can place any headers you want to use in here. The compiler knows to look in this directory, so you can define any headers here. I dont think you can nest in subdirectories though but if you want to it should be trivial to modify the build script or you can always open an issue and id be happy to add that.

`yoyo_c_api.h` is an interesting workaround to a cmake issue. We cant define macros inside of source files, so you will need to open this header and uncomment any of the macros for implementing api functions. I will provide an example of this below if that didnt make sense.

## Scripting

Any Scripting you do in C will be done through interfacing api functions extended by the engine and game template. If you really want to, you could modify `entry.c` to customize some behavior, but practically speaking anything you could change in there can also be done through the C api.

To get started, lets assume we made a file called `custom/src/game.c` and we want to print out "Hello Yoyo Engine!" when the engine starts.
Taking a look at the contents of `custom/include/yoyo_c_api.h` we can see that there is a macro called `YOYO_POST_INIT` that lets the engine know that we have implemented the signature `yoyo_post_init()`.

Knowing this, all we need to do is uncomment the macro defining that function, and then actually implement it somewhere in our source. Let's go into `game.c` and do so now.

```c
#include "yoyo_c_api.h"

void yoyo_post_init(){
    printf("Hello Yoyo Engine!\n");
}
```

And its really as simple as that. Taking a peek at `yoyo_c_api.h` you can see more information on what callbacks are available to you and when they are run, which can be combined in such a manner that you can achieve any behavior you want.

## Parting Notes

Is this not enough to achieve what you want?
Keep in mind that you can also use lua scripting to more directly and easily interface with the engine, but you might not have the same level of control as C. Does that also not solve your problem? The final option open to you is writing a custom Trick (The engines name for a plugin). More information on that follows in the next section.
