"""
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
"""

"""
    Script to assist in building a launcher game.
    Generates a CMakeLists.txt and runs the build commands.
"""

import os
import sys
import shutil
import subprocess
import json

# set some variables
script_version = "1.0.0"

print("----------------------------------")
print("Yoyo Engine Build Script v" + script_version)
print("Ryan Zmuda 2023")
print("----------------------------------")

script_directory = os.path.dirname(os.path.abspath(__file__))
os.chdir(script_directory)
current_dir = os.getcwd()

# open settings.yoyo (json) and build.yoyo (json) and extract their contents so we can read them
settings_file = open("settings.yoyo", "r")
settings = json.load(settings_file)
settings_file.close()

build_file = open("build.yoyo", "r")
build = json.load(build_file)
build_file.close()

# Get details on the build and game (game name spaces->underscores)
game_name = settings["name"].replace(" ", "_")

build_platform = build["platform"]
build_cflags = build["cflags"]
build_engine_path = build["engine_build_path"] + "/" + build_platform

shpath = build["engine_build_path"] + "/../"

# temp - call build_linux.sh or build_windows.sh depending on platform
if(build_platform == "linux"):
    subprocess.run([shpath+"build_linux.sh"], cwd=shpath)
else:
    subprocess.run([shpath+"build_windows.sh"], cwd=shpath)

# Check if the engine build path exists
if not os.path.exists(build_engine_path):
    print("Error: Engine build path \"" + build_engine_path + "\" does not exist.")
    print("Please set the engine build path in build.yoyo to the path of the engine build folder you want to use.")
    sys.exit()

# Create a build folder
if os.path.exists("./build"):
    shutil.rmtree("./build")
os.mkdir("./build")

# Populate our build with the engine build and custom files from the project.
# (THIS DOES NOT INCLUDE SOURCE FILES FROM THE PROJECT)
shutil.copytree(build_engine_path, "./build/" + build_platform)
shutil.copytree("./custom/include", "./build/" + build_platform + "/include", dirs_exist_ok=True)
shutil.copytree("./custom/lib", "./build/" + build_platform + "/lib", dirs_exist_ok=True)
shutil.copytree("./resources", "./build/" + build_platform + "/resources", dirs_exist_ok=True)
shutil.copyfile("./settings.yoyo", "./build/" + build_platform + "/settings.yoyo")

print("Building \"" + game_name + "\" for " + build_platform + " with flags: " + build_cflags)

# create a toolchain-win.cmake file in that folder
toolchain_file = open("./build/toolchain-win.cmake", "w")
toolchain_file.write("set(CMAKE_SYSTEM_NAME Windows)\nset(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)\nset(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)\nset(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)\nset(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)")
toolchain_file.close()

#
# WE ARE GOING TO BUILD ALL TRICKS INTO build/platform/tricks
# optional: later on we can also construct a tricks.yoyo file to compact info on each one

# create the tricks folder
os.mkdir("./build/" + build_platform + "/tricks")

# create a tricks.yoyo file in that folder
tricks_file = open("./build/" + build_platform + "/tricks/tricks.yoyo", "w")

# write {"tricks":[]} to set it up
tricks_file.write("{\"tricks\":[")

# loop through each folder in ./tricks
for trick in os.listdir("./tricks"):
    # if this is not a directory, skip it
    if not os.path.isdir("./tricks/" + trick):
        continue

    # look at its trick.yoyo file and get the name of the trick
    trick_file = open("./tricks/" + trick + "/trick.yoyo", "r")
    trick_data = json.load(trick_file)
    trick_file.close()

    trick_name = trick_data["name"]
    trick_description = trick_data["description"]
    trick_author = trick_data["author"]
    trick_version = trick_data["version"]

    print("----------------------------------")
    print("Building trick \"" + trick_name + "\"...")
    print("Author: " + trick_author)
    print("Version: " + trick_version)
    print(trick_description)
    print("----------------------------------")

    # delete the trick's build folder if it exists
    if os.path.exists("./tricks/" + trick + "/build"):
        shutil.rmtree("./tricks/" + trick + "/build")

    # create a build folder in the trick folder
    os.mkdir("./tricks/" + trick + "/build")

    # copy the trick's include/ and lib/ folders into the build folder
    shutil.copytree("./tricks/" + trick + "/include", "./tricks/" + trick + "/build/include", dirs_exist_ok=True)
    shutil.copytree("./tricks/" + trick + "/lib", "./tricks/" + trick + "/build/lib", dirs_exist_ok=True)

    # create a CMakeLists.txt file in that folder
    cmake_file = open("./tricks/" + trick + "/build/CMakeLists.txt", "w")

    # write to that file the CMakeLists.txt template
    cmake_file.write("cmake_minimum_required(VERSION 3.22.1)\n")

    cmake_file.write("project(" + trick_name + ")\n")

    cmake_file.write("set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} "+build_cflags+"\")\n")

    cmake_file.write("file(GLOB SOURCES \""+current_dir+"/tricks/"+trick+"/src/*.c\")\n")

    cmake_file.write("include_directories("+current_dir+"/tricks/"+trick+"/build/include)\n")
    build_dir = current_dir + "/build/" + build_platform
    cmake_file.write("include_directories("+build_dir+"/include)\n")

    cmake_file.write("set(EXECUTIBLE_NAME " + trick_name + ")\n")

    cmake_file.write("add_library(${EXECUTIBLE_NAME} SHARED ${SOURCES})\n")

    # run cmake
    extension = ""
    if(build_platform == "linux"):
        extension = ".so"
    elif(build_platform == "windows"):
        extension = ".dll"

    cmake_file.write("file(GLOB LIB_FILES "+current_dir+"/tricks/"+trick+"/build/lib/"+build_platform+"/*"+extension+")\n")

    cmake_file.write("target_link_directories(${EXECUTIBLE_NAME} PRIVATE "+current_dir+"/tricks/"+trick+"/build/lib)\n")

    cmake_file.write("target_link_libraries(${EXECUTIBLE_NAME} PRIVATE ${LIB_FILES})\n")

    cmake_file.close()

    # print the current working directory of python for debug purposes
    print("Current working directory: " + os.getcwd())

    # run cmake
    if(build_platform == "linux"):
        subprocess.run(["cmake", "-S", "./tricks/" + trick + "/build", "-B", "./tricks/" + trick + "/build/" + build_platform])
    elif(build_platform == "windows"):
        subprocess.run(["cmake", "-DCMAKE_TOOLCHAIN_FILE="+current_dir+"/build/toolchain-win.cmake", "-S", "./tricks/" + trick + "/build", "-B", "./tricks/" + trick + "/build/" + build_platform])

    # run make
    subprocess.run(["make", "-C", "./tricks/" + trick + "/build/" + build_platform])

    # copy the trick's .so or .dll file into ./build/tricks
    shutil.copyfile("./tricks/" + trick + "/build/" + build_platform + "/lib" + trick_name + extension, "./build/" + build_platform + "/tricks/lib" + trick_name + extension)

    # copy the trick's include/ to ./build/<platform>/include
    shutil.copytree("./tricks/" + trick + "/build/include", "./build/" + build_platform + "/include", dirs_exist_ok=True)

    # copy the trick's lib/ to ./build/<platform>/lib
    try:
        if os.path.exists("./tricks/" + trick + "/build/lib/"+build_platform) and os.listdir("./tricks/" + trick + "/build/lib/"+build_platform):
            shutil.copytree("./tricks/" + trick + "/build/lib/"+build_platform, "./build/" + build_platform + "/lib", dirs_exist_ok=True)
        else:
            print("No supplemental libraries found in ./tricks/" + trick + "/build/lib/"+build_platform)
    except Exception as e:
        print(f"An error occurred: {e}")

    # figure out the tricks file name with extension and lib prefix included
    trick_filename = "lib" + trick_name + extension

    # populate the tricks.yoyo file with the trick's info, the structure is "tricks":[{trick1}, {trick2}, ...]
    tricks_file.write("{\"name\":\"" + trick_name + "\",\"description\":\"" + trick_description + "\",\"author\":\"" + trick_author + "\",\"version\":\"" + trick_version + "\",\"filename\":\"" + trick_filename + "\"},")

# remove the last comma from the tricks.yoyo file
tricks_file.seek(tricks_file.tell() - 1, os.SEEK_SET)
tricks_file.truncate()

# close the tricks.yoyo file
tricks_file.write("]}")
tricks_file.close()

#
# Resume building game
#

# create a CMakeLists.txt file in that folder
cmake_file = open("./build/CMakeLists.txt", "w")

# write to that file the CMakeLists.txt template
cmake_file.write("cmake_minimum_required(VERSION 3.22.1)\n")

cmake_file.write("project(" + game_name + ")\n")

cmake_file.write("set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} "+build_cflags+"\")\n")

cmake_file.write("set(SOURCES \""+current_dir+"/entry.c\")\n")
cmake_file.write("file(GLOB CUSTOM_SOURCES \""+current_dir+"/custom/src/*.c\")\n")

build_dir = current_dir + "/build/" + build_platform

cmake_file.write("include_directories("+build_dir+"/include)\n")

cmake_file.write("set(EXECUTIBLE_NAME " + game_name + ")\n")

cmake_file.write("add_executable(${EXECUTIBLE_NAME} ${SOURCES} ${CUSTOM_SOURCES})\n")

if build_platform == "windows":
    cmake_file.write("file(GLOB LIB_FILES "+build_dir+"/lib/*.dll)\n")
elif build_platform == "linux":
    cmake_file.write("file(GLOB LIB_FILES "+build_dir+"/lib/*.so)\n")
else:
    print("Unsupported build platform")

cmake_file.write("target_link_directories(${EXECUTIBLE_NAME} PRIVATE "+build_dir+"/lib)\n")

cmake_file.write("target_link_libraries(${EXECUTIBLE_NAME} PRIVATE ${LIB_FILES} SDL2_mixer SDL2_ttf SDL2_image lua jansson yoyoengine png16 zlib1)\n")

cmake_file.close()

print("----------------------------------")
print("Running cmake...")
print("----------------------------------")

# run cmake
if(build_platform == "linux"):
    subprocess.run(["cmake", "-S", "./build", "-B", "./build/" + build_platform])
elif(build_platform == "windows"):
    subprocess.run(["cmake", "-DCMAKE_TOOLCHAIN_FILE=../toolchain-win.cmake", "-S", "./build", "-B", "./build/" + build_platform])
else:
    print("Error: Unknown platform \"" + build_platform + "\"")
    sys.exit()

print("----------------------------------")
print("Running make...")
print("----------------------------------")

# run make
subprocess.run(["make", "-C", "./build/" + build_platform])

# if windows, we need to copy the dlls to the build folder and delete /lib
if(build_platform == "windows"):
    shutil.copyfile("./build/" + build_platform + "/lib/libSDL2.dll", "./build/" + build_platform + "/SDL2.dll")
    shutil.copyfile("./build/" + build_platform + "/lib/libSDL2_image.dll", "./build/" + build_platform + "/SDL2_image.dll")
    shutil.copyfile("./build/" + build_platform + "/lib/libSDL2_mixer.dll", "./build/" + build_platform + "/SDL2_mixer.dll")
    shutil.copyfile("./build/" + build_platform + "/lib/libSDL2_ttf.dll", "./build/" + build_platform + "/SDL2_ttf.dll")
    shutil.copyfile("./build/" + build_platform + "/lib/liblua.dll", "./build/" + build_platform + "/lua54.dll")
    shutil.copyfile("./build/" + build_platform + "/lib/libjansson.dll", "./build/" + build_platform + "/libjansson-4.dll")
    shutil.copyfile("./build/" + build_platform + "/lib/libyoyoengine.dll", "./build/" + build_platform + "/libyoyoengine.dll")
    shutil.copyfile("./build/" + build_platform + "/lib/libfreetype-6.dll", "./build/" + build_platform + "/libfreetype-6.dll")
    shutil.copyfile("./build/" + build_platform + "/lib/libmpg123-0.dll", "./build/" + build_platform + "/libmpg123-0.dll")
    shutil.copyfile("./build/" + build_platform + "/lib/libpng16-16.dll", "./build/" + build_platform + "/libpng16-16.dll")
    shutil.copyfile("./build/" + build_platform + "/lib/libjpeg-9.dll", "./build/" + build_platform + "/libjpeg-9.dll")
    shutil.copyfile("./build/" + build_platform + "/lib/zlib1.dll", "./build/" + build_platform + "/zlib1.dll")
    shutil.rmtree("./build/" + build_platform + "/lib")
    print("Copied dlls to build folder.")

# print out the executible we built
print("----------------------------------")
print("Built Executible:\n" + current_dir + "/build/" + build_platform + "/" + game_name)

print("Cleaning up Cmake artifacts...")

# remove the CMakeLists.txt file, the CMakeFiles folder, and the cmake_install.cmake file and the CMakeCache.txt file and the Makefile
os.remove("./build/CMakeLists.txt")
os.remove("./build/toolchain-win.cmake")
shutil.rmtree("./build/" + build_platform + "/CMakeFiles")
os.remove("./build/" + build_platform + "/cmake_install.cmake")
os.remove("./build/" + build_platform + "/CMakeCache.txt")
os.remove("./build/" + build_platform + "/Makefile")

print("Cleaning up headers...")

# remove /build/<platform>/include
shutil.rmtree("./build/" + build_platform + "/include")

print("----------------------------------")

# print out "done" in green
print("\033[92m" + "Done!" + "\033[0m")

# if we received a command line argument, run the game
if len(sys.argv) > 1:
    if sys.argv[1] == "run":
        print("\n----------------------------------")
        print("Running game...")
        print("----------------------------------")
        if(build_platform == "linux"):
            subprocess.Popen(["./build/" + build_platform + "/" + game_name])
        elif(build_platform == "windows"):

            # if this script is being run on linux, open this with wine
            if sys.platform == "linux":
                subprocess.Popen(["wine", "./build/" + build_platform + "/" + game_name + ".exe"])
            else:
                subprocess.Popen([current_dir+"/build/" + build_platform + "/" + game_name + ".exe"])