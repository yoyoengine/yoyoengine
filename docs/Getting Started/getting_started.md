# Getting Started

The reccomended way to use yoyoengine is via a submodule inside your main git project, where you build and run the tooling yourself.

In the future I might release standalone versions of the editor and launcher, but by the nature of yoyoengine, you will probably want to hack on the core engine itself at some point.

## Adding yoyoengine

1. Create a new git repo
2. Run the command `git submodule add https://github.com/zoogies/yoyoengine.git` in the root of the project
3. Navigate into `/yoyoengine/editor` and run `build_linux.sh` to create the editor binary
4. Navigate into `/yoyoengine/launcher` and run the command `python3 launcher.py` to open the project manager
5. Using the project manager, select the location of the editor binary, and then create a new project inside your repo
6. The editor should automatically launch into a default scene. Done!

!!! info
    This method relies on you having the necessary python dependencies installed on your system for the launcher. You can just as easily create a new folder, and copy over `yoyoengine/launcher/template/*` to get a base project.
