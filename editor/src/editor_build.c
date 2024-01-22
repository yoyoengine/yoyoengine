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

void editor_build(){
    // pack the /engine_resources into a .yep file
    yep_pack_directory(ye_get_engine_resource_static(""), ye_get_resource_static("../engine.yep"));

    // pack the /resources into a .yep file
    yep_pack_directory(ye_get_resource_static(""), ye_get_resource_static("../resources.yep"));

    // call the build script
    char command[256];
    snprintf(command, sizeof(command), "python3 \"%s\"", ye_get_resource_static("../build.py"));
    printf("command: %s\n", command);
    system(command);
}

void editor_build_and_run(){
    // pack the /engine_resources into a .yep file
    yep_pack_directory(ye_get_engine_resource_static(""), ye_get_resource_static("../engine.yep"));

    // pack the /resources into a .yep file
    yep_pack_directory(ye_get_resource_static(""), ye_get_resource_static("../resources.yep"));

    // call the build script
    char command[256];
    snprintf(command, sizeof(command), "python3 \"%s\" --run", ye_get_resource_static("../build.py"));
    system(command);
}