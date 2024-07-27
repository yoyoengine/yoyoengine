# Getting Started

The reccomended way to use yoyoengine is via a submodule inside your main git project, where you build and run the tooling yourself.

In the future I might release standalone versions of the editor and launcher, but by the nature of yoyoengine, you will probably want to hack on the core engine itself at some point.

## Dependencies

There are a few "hard" dependencies for yoyoengine, which are:

- A C compiler, CMake, Make, and git

But there are also some "soft" dependencies which are:

- curl
- zenity

These "soft" dependencies will cause runtime "errors" if they are not present, but everything will still work as expected.

For example, yoyoeditor uses curl to check for updates against github, but if curl is not installed you will simply see "curl is not installed." in the console without any other issues.

These are typically installed on every linux system by default, so you shouldn't have to worry about them.

## Adding yoyoengine

1. Create a new git repo
2. Run the command `git submodule add https://github.com/zoogies/yoyoengine.git` in the root of the project
3. Navigate into `/yoyoengine/editor` and run `build_linux.sh` to create the editor binary
4. Navigate into `/yoyoengine/launcher` and run the command `python3 launcher.py` to open the project manager
5. Using the project manager, select the location of the editor binary, and then create a new project inside your repo
6. The editor should automatically launch into a default scene. Done!

!!! info
    This method relies on you having the necessary python dependencies installed on your system for the launcher. You can just as easily create a new folder, and copy over `yoyoengine/launcher/template/*` to get a base project.
