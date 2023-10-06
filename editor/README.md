# Yoyo Engine Editor

The editor is a visual editor for the engine. It will essentially be a "game" running the core framework to provide an easy way to layout scenes, and tools to build them together without writing the framework interfacing code yourself.

While the editor does interface with some internal editor specific logic in the engine, most of these have been exposed as boolean flags in the `engine_state` and `engine_runtime_state` structs and can be modified at the developers leasure hopefully to provide some flexibility.

## Notices

`.yoyo` files are really just `.json` files, but I wanted to rename them to feel cool :)

## Project Structure

The launcher will create a project directory with the following structure:

```txt
.
├── resources
│   ├── scenes
│   │   └── scene1.yoyo
│   └── style.yoyo
└── settings.yoyo
```

`settings.yoyo` contains the metadata for the project, including some engine specific parameters for compilation

`/resources` contains a `style.yoyo` file which describes all declared fonts and colors to be used in the project.

`/resources/scenes` contains all the scenes in the project. Scenes are `.yoyo` files which contain a list of entities and components.

## build system

should actually be pretty easy now that we are using cmake.

1. Get the build parameters from the project file
   1. Platform
   2. Compiler (prob force gcc)
   3. Any extra C files they want to include. The entry point might also need auto generated, unless we make a generic entry point which loads params from file. This is prob the best choice. It has gotten absurdly messy to try to pass everything in as flags with overrides.
2. Generate a `CMakeLists.txt` file
3. Run cmake
4. Build the project
5. Thats it!

## afterword to future ryan

I dont feel like porting this to be a true project structure just yet, ill wait until that structure is actually fully completed.
