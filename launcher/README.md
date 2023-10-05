# Launcher

## Created Project Structure

```txt
.
├── build.yoyo
├── custom
│   ├── include
│   ├── lib
│   └── src
├── resources
│   ├── scenes
│   │   └── entry.yoyo
│   └── styles.yoyo
└── settings.yoyo
```

### build.yoyo

json file that describes the build parameters.

### custom

This folder is used to store custom C scipts and headers, as well as libraries.

### resources

This is the folder that any asset or file you want to use in the game must be placed in. The `scenes` folder is where you place your game's scenes, and the `styles.yoyo` file describes declared fonts and colors for the game.

### settings.yoyo

This json file describes the launch settings for the game as well as the game's name and version, among other parameters.

## What happens when building

When building, the following happens:

1. a build folder is created (and an old build folder is deleted if needed).
2. your current engine version (whichever you launched the editor with) is copied to the build folder.
3. your project tree is copied to the build folder, but with any lib or includes in the `custom` folder being copied to the `lib` and `include` folders in the engines build folder.
4. A CmakeLists.txt file is created in the build folder, based on your project's settings.
5. Cmake is run on the build folder, which generates a makefile.
6. Make is run on the build folder, which compiles the project.
