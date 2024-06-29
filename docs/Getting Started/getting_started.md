# Getting Started

The reccomended way to use yoyoengine is via a submodule inside your main git project, where you build and run the tooling yourself.
In the future I might release standalone versions of the editor and launcher, but for now that hasnt happened.

1. Create a new git repo
2. Run the command `git submodule add https://github.com/zoogies/yoyoengine.git` in the root of the project
3. Navigate into `/yoyoengine/editor` and run `build_linux.sh` to create the editor binary
4. Navigate into `/yoyoengine/launcher` and run the command `python3 launcher.py` to open the project manager
5. Using the project manager, select the location of the editor binary, and then create a new project inside your repo
6. The editor should automatically launch into a default scene. Done!
