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
