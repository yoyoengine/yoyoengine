/*
    This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <yoyoengine/yoyoengine.h>

void editor_build_packs(){
    // engine resources base dir
    char *_engine_resources = ye_get_engine_resource_static("");
    char *engine_resources = malloc(strlen(_engine_resources) + 1);
    strcpy(engine_resources, _engine_resources);

    // resources base dir
    char *_resources = ye_get_resource_static("");
    char *resources = malloc(strlen(_resources) + 1);
    strcpy(resources, _resources);

    // engine.yep path
    char *_engine_yep = ye_get_resource_static("../engine.yep");
    char *engine_yep = malloc(strlen(_engine_yep) + 1);
    strcpy(engine_yep, _engine_yep);

    // resources.yep path
    char *_resources_yep = ye_get_resource_static("../resources.yep");
    char *resources_yep = malloc(strlen(_resources_yep) + 1);
    strcpy(resources_yep, _resources_yep);

    // pack the /engine_resources into a .yep file
    yep_pack_directory(engine_resources, engine_yep);

    // pack the /resources into a .yep file
    yep_pack_directory(resources, resources_yep);

    // free the memory
    free(engine_resources);
    free(resources);
    free(engine_yep);
    free(resources_yep);

}

void editor_build(){

    editor_build_packs();    

    // call the build script
    char command[256];
    snprintf(command, sizeof(command), "python3 \"%s\"", ye_get_resource_static("../build.py"));
    printf("command: %s\n", command);
    system(command);
}

void editor_build_and_run(){

    editor_build_packs();

    // call the build script
    char command[256];
    snprintf(command, sizeof(command), "python3 \"%s\" --run", ye_get_resource_static("../build.py"));
    system(command);
}