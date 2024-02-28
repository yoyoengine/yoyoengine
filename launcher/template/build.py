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

import os
import sys
import shutil
import subprocess
import json

# returns whether we specified some cli args
def parse_args():
    args = sys.argv[1:]
    return '--run' in args, '--run-only' in args, '--clean' in args, '--delete-cache' in args, '--reconfigure' in args

# cleans a directory, excluding a single file or directory
def clean_directory(path, exclude):
    for file in os.listdir(path):
        if file != exclude:
            file_path = os.path.join(path, file)
            if os.path.isfile(file_path):
                os.remove(file_path)  # remove the file
            elif os.path.isdir(file_path):
                shutil.rmtree(file_path)  # remove dir and all contains

class YoyoEngineBuildSystem:
    def __init__(self, script_version="v2.0.0"):
        self.script_version = script_version

        # get cli args
        self.run_flag, self.runonly_flag, self.clean_flag, self.delete_cache, self.reconfigure = parse_args()
        
        # chdir to where the script is located (to access things relatively)
        self.script_location = os.path.dirname(os.path.abspath(__file__))
        os.chdir(self.script_location)

        # get the game data
        with open("settings.yoyo", "r") as file:
            self.game_settings = json.load(file)

        # get the build settings
        with open("build.yoyo", "r") as file:
            self.build_settings = json.load(file)

        self.game_name = self.game_settings["name"].replace(" ", "_")
        self.game_platform = self.build_settings["platform"]
        self.build_cflags = self.build_settings["cflags"]
        self.build_rc_path = self.build_settings["rc_path"]
        
        if self.runonly_flag:
            return

        # add the -mwindows flag to disable console TODO: only if not debug build which isnt checked now
        if self.game_platform == "windows":
            self.build_cflags += " -mwindows"
        elif self.game_platform == "emscripten":
            self.build_cflags += " --emrun -Wcast-function-type -Wbad-function-cast"
        
        # if self.game_settings["debug_mode"] == True, add -g, -Wall, and -Wextra to the build flags
        if self.build_settings["build_mode"] == "debug":
            self.build_cflags += " -g -Wall -Wextra"
        elif self.build_settings["build_mode"] == "release":
            self.build_cflags += " -s -O3" # optimize heavily

        # we need the path to the engine, but only to specify its CMakeLists.txt
        self.build_engine_path = self.build_settings["engine_build_path"]
        # Check if the engine build path exists
        if not os.path.exists(self.build_engine_path):
            print("[YOYO BUILD] Error: Engine build path \"" + self.build_engine_path + "\" does not exist.")
            print("[YOYO BUILD] Please set the engine build path in build.yoyo to the path of the engine build folder you want to use.")
            sys.exit()
        
        # if this flag is set, we have changed target platforms from what we are already configured for. We need to delete everything BUT out/_deps
        if(self.build_settings['delete_cache'] == True or self.delete_cache == True):
            print("[YOYO BUILD] Deleting cache...")
            # delete everything (files and folders recursively) in the build folder except build/out
            if os.path.exists("./build"):
                clean_directory("./build", "out")

            # delete everything (files and folders recursively) in the build/out folder except build/out/_deps
            if os.path.exists("./build/out"):
                clean_directory("./build/out", "_deps")

            # reset the flag
            self.build_settings['delete_cache'] = False
            # update the build.yoyo file
            with open("build.yoyo", "w") as file:
                json.dump(self.build_settings, file, indent=4)
                print("[YOYO BUILD] Cache deleted and flag reset.")

        # ONLY if we HAVENT cleaned cache in this last step, we need to check if we got the cli arg to totally clean
        # if we recieved arg --clean OR there is no /build/out dir existant, we need to fresh configure and build
        # TODO: this is probably redundant since last step keeps the deps and reconfigures
        elif self.clean_flag or not os.path.exists("./build/out"):
            # Create a build/out folder
            if os.path.exists("./build/out"):
                shutil.rmtree("./build/out")
            os.makedirs("./build/out")

        # set self.binary_dir depending on the platform so we dont have to do this in a bunch of separate places
        if self.game_platform == "linux":
            self.binary_dir = "bin/Linux"
            self.cmake_platform_name = "Linux"
        elif self.game_platform == "windows":
            self.binary_dir = "bin/Windows"
            self.cmake_platform_name = "Windows"
        elif self.game_platform == "emscripten":
            self.binary_dir = "bin/Emscripten"
            self.cmake_platform_name = "Emscripten"
        
        # check if cmake artifacts exist in the build folder, otherwise set self.reconfigure to True
        if not os.path.exists("./build/CMakeLists.txt"):
            self.reconfigure = True
    
    def configure(self):
        # write our CMakeLists.txt file and run cmake .. to configure
        print("[YOYO BUILD] Configuring \"" + self.game_name + "\" for " + self.game_platform + " with flags: " + self.build_cflags)
        
        # Ensure the build directory exists
        if not os.path.exists("./build"):
            os.makedirs("./build")

        # Ensure the build/out directory exists
        if not os.path.exists("./build/out"):
            os.makedirs("./build/out")

        # create a CMakeLists.txt file in that folder
        cmake_file = open("./build/CMakeLists.txt", "w")

        # write the contents
        cmake_file.write(f"""
        ############################################################################
        # This file was automatically generated by Yoyo Engine Build System {self.script_version} #
        ############################################################################

        cmake_minimum_required(VERSION 3.22.1)
        project({self.game_name})
        """)
        # if in release mode, set YOYO_ENGINE_BUILD_RELEASE to on cache bool
        if self.build_settings["build_mode"] != "release":
            cmake_file.write(f"set(YOYO_ENGINE_BUILD_RELEASE OFF CACHE BOOL \"Enable Release mode\" FORCE)\n")
        else:
            cmake_file.write(f"set(YOYO_ENGINE_BUILD_RELEASE ON CACHE BOOL \"Enable Release mode\" FORCE)\n")
        
        cmake_file.write(f"""

        add_subdirectory("{self.build_engine_path}" yoyoengine)
        include_directories(include ${{CMAKE_BINARY_DIR}}/bin/${{CMAKE_SYSTEM_NAME}}/include)

        set(CMAKE_C_FLAGS "${{CMAKE_C_FLAGS}} {self.build_cflags}")

        set(SOURCES "{self.script_location}/entry.c")

        file(GLOB CUSTOM_SOURCES "{self.script_location}/custom/src/*.c")

        ## rpath on linux ##
        SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
        SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
        SET(CMAKE_INSTALL_RPATH $ORIGIN/lib)
        ####################

        ## app icon stuff ##
        add_executable({self.game_name} ${{SOURCES}} ${{CUSTOM_SOURCES}} {self.script_location}/{self.build_rc_path})
        ####################

        # TODO: add optimization flags and stripping for binary here

        # lets the tricks know where to link against any deps that the game has
        set(YOYO_GAME_LINK_DIR ${{CMAKE_BINARY_DIR}}/bin/${{CMAKE_SYSTEM_NAME}}/lib)

        target_link_directories({self.game_name} PRIVATE ${{CMAKE_BINARY_DIR}}/bin/${{CMAKE_SYSTEM_NAME}}/tricks)
        """)
        # target_link_directories({self.game_name} PRIVATE ${{CMAKE_BINARY_DIR}}/bin/${{CMAKE_SYSTEM_NAME}}/lib)
        # target_link_directories({self.game_name} PRIVATE ${{CMAKE_BINARY_DIR}}/bin/${{CMAKE_SYSTEM_NAME}}/lib/..)

        # for some reason, we need to add our tricks right here so it does not conflict with the build order
        cmake_file.write(f"""\n        # BUILD TRICKS:

        set(YOYO_TRICK_BUILD_DIR ${{CMAKE_BINARY_DIR}}/bin/${{CMAKE_SYSTEM_NAME}}/tricks)
        file(MAKE_DIRECTORY ${{YOYO_TRICK_BUILD_DIR}})

        file(COPY "{self.script_location}/tricks.yoyo" DESTINATION ${{YOYO_TRICK_BUILD_DIR}})\n\n""")

        """
        Handle tricks:

        1. loop through each trick in the tricks folder, and add_subdirectory its path
        2. add the tricks details to the generated tricks.yoyo file
        """

        # open (create) the tricks.yoyo file in self.binary_dir
        tricks_file = open(f"{self.script_location}/tricks.yoyo", "w")

        tricks_data = {"WARNING":"THIS FILE IS AUTO-GENERATED! DO NOT TAMPER!","tricks": []}

        # Lets start by looping through each trick in the tricks folder, and invoking add_subdirectory to its path
        for trick in os.listdir("./tricks"):
            # if this is not a directory, skip it
            if not os.path.isdir("./tricks/" + trick):
                continue

            # write the name
            cmake_file.write(f"        # {trick}\n")
            # write the add_subdirectory command
            cmake_file.write(f"        add_subdirectory({self.script_location}/tricks/{trick} trick_builds/{trick})\n")
            # include a default path in that trick
            cmake_file.write(f"        target_include_directories({self.game_name} PRIVATE {self.script_location}/tricks/{trick}/include)\n")
            # make sure this happens after the game is built
            cmake_file.write(f"        add_dependencies({trick} yoyoengine)\n")
            # link the trick to the game
            cmake_file.write(f"        target_link_libraries({self.game_name} PRIVATE {trick})\n\n")

            # look at its trick.yoyo file and get the name of the trick
            trick_file = open("./tricks/" + trick + "/trick.yoyo", "r")
            trick_data = json.load(trick_file)
            trick_file.close()

            trick_name = trick_data["name"]
            trick_description = trick_data["description"]
            trick_author = trick_data["author"]
            trick_version = trick_data["version"]

            # Add the trick data to the tricks_data dictionary
            tricks_data["tricks"].append({
                "name": trick_name,
                "description": trick_description,
                "author": trick_author,
                "version": trick_version
            })

        # Write the tricks_data to the tricks_file in JSON format
        json.dump(tricks_data, tricks_file, indent=4)

        tricks_file.close()

        cmake_file.write(f"""\n        # DONE BUILDING TRICKS\n""")

        cmake_file.write(f"""
        include_directories({self.script_location}/custom/include)

        target_link_libraries({self.game_name} PRIVATE yoyoengine)

        set_target_properties({self.game_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${{CMAKE_BINARY_DIR}}/bin/${{CMAKE_SYSTEM_NAME}})

        file(COPY "../settings.yoyo" DESTINATION ${{CMAKE_BINARY_DIR}}/bin/${{CMAKE_SYSTEM_NAME}})
        file(COPY "../engine.yep" DESTINATION ${{CMAKE_BINARY_DIR}}/bin/${{CMAKE_SYSTEM_NAME}})
        file(COPY "../resources.yep" DESTINATION ${{CMAKE_BINARY_DIR}}/bin/${{CMAKE_SYSTEM_NAME}})        
        """)

        if(self.game_platform == "emscripten"):
            cmake_file.write(f"""
        
        # emscripten specific settings
        target_link_options({self.game_name} PRIVATE --bind -Wbad-function-cast -Wcast-function-type -sALLOW_MEMORY_GROWTH=1 -sMAXIMUM_MEMORY=1gb --preload-file {self.script_location}/build/out/{self.binary_dir}@/)
        set(CMAKE_EXECUTABLE_SUFFIX \".html\")
            """)
        # target_link_options({self.game_name} PRIVATE --bind -Wbad-function-cast -Wcast-function-type -sALLOW_MEMORY_GROWTH=1 -sMAXIMUM_MEMORY=1gb --preload-file {self.script_location}/build/out/{self.binary_dir}/settings.yoyo@/ --preload-file {self.script_location}/build/out/{self.binary_dir}/engine.yep/@ --preload-file {self.script_location}/build/out/{self.binary_dir}/resources.yep/@)
        cmake_file.close()

        # create all toolchains we might need in the build folder
        toolchain_file = open("./build/toolchain-win.cmake", "w")
        toolchain_file.write("set(CMAKE_SYSTEM_NAME Windows)\nset(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)\nset(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)\nset(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)\nset(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)")
        toolchain_file.close()

        toolchain_file = open("./build/toolchain-emscripten.cmake", "w")
        toolchain_file.write(f"""
        set(CMAKE_SYSTEM_NAME Emscripten)
        set(CMAKE_C_COMPILER \"emcc\")
        set(CMAKE_CXX_COMPILER \"em++\")
        set(CMAKE_CXX_FLAGS \"--bind\")
        set(CMAKE_C_FLAGS \"\")
        set(EMSCRIPTEN ON CACHE BOOL \"Enable Emscripten toolchain\" FORCE)
        """)
        toolchain_file.close()

        # run cmake
        print("----------------------------------")
        print("Running cmake...")

        # chdir into the build folder
        os.chdir("./build/out")

        # run cmake
        if(self.game_platform == "linux"):
            print("[YOYO BUILD] Running cmake FOR LINUX")
            subprocess.run(["cmake", ".."])
        elif(self.game_platform == "windows"):
            print("[YOYO BUILD] Running cmake FOR WINDOWS")
            subprocess.run(["cmake", "-DCMAKE_TOOLCHAIN_FILE=./toolchain-win.cmake", ".."])
        elif(self.game_platform == "emscripten"):
            print("[YOYO BUILD] Running cmake FOR EMSCRIPTEN")
            subprocess.run(["cmake", "-DCMAKE_TOOLCHAIN_FILE=./toolchain-emscripten.cmake", ".."])
        else:
            print("Error: Unknown platform \"" + self.game_platform + "\"")
            sys.exit()

        print("----------------------------------")
    
    def build(self):
        print("----------------------------------")
        print("Running make...")
        print("----------------------------------")

        # run make
        if(self.game_platform == "emscripten"):
            subprocess.run(["emmake", "make"])  # Use 4 threads
        else:
            subprocess.run(["make"])  # Use 4 threads

        # on windows, we need to copy the dlls from /lib into the executible dir
        if(self.game_platform == "windows"):
            shutil.copytree("./bin/Windows/lib/", "./bin/Windows/", dirs_exist_ok=True)
            shutil.rmtree("./bin/Windows/lib")
            shutil.copytree("./bin/Windows/tricks/", "./bin/Windows/", dirs_exist_ok=True)
            shutil.rmtree("./bin/Windows/tricks")
            os.remove("./bin/Windows/tricks.yoyo")
            os.remove("./bin/Windows/libyoyoengine.dll.a")
            print("[YOYO BUILD] Copied dlls to build folder.")
        elif(self.game_platform == "emscripten"):
            # copy only .data files from lib into root
            for file in os.listdir("./bin/Emscripten/lib"):
                if file.endswith(".data"):
                    shutil.copy(f"./bin/Emscripten/lib/{file}", "./bin/Emscripten/")
            
            # delete tricks, lib, resources.yep, engine.yep
            shutil.rmtree("./bin/Emscripten/tricks")
            shutil.rmtree("./bin/Emscripten/lib")
            print("[YOYO BUILD] Cleaned up Emscripten build folder.")

        # build cleanup, remove the include folder in the output
        if(os.path.exists(f"./bin/{self.cmake_platform_name}/include")):
            shutil.rmtree(f"./bin/{self.cmake_platform_name}/include")
        print("[YOYO BUILD] Removed include folder from build folder.")

        if(self.game_platform == "emscripten"):
            # dont need local copies, wasm linker has packed them into .data
            os.remove("./bin/Emscripten/resources.yep")
            os.remove("./bin/Emscripten/engine.yep")
            os.remove("./bin/Emscripten/settings.yoyo")

            # rename the .html file to index.html, because itch and other html5 sites expect this
            os.rename(f"./bin/Emscripten/{self.game_name}.html", f"./bin/Emscripten/index.html")

    def run(self):
        # if we recieved arg --run, run the game
        if self.run_flag:
            print("\n----------------------------------")
            print("Running game...")
            print("----------------------------------")
            if(self.game_platform == "linux"):
                subprocess.Popen(["./bin/Linux/" + self.game_name])
            elif(self.game_platform == "windows"):
                # if this script is being run on linux, open this with wine
                if sys.platform == "linux":
                    subprocess.Popen(["wine", "./bin/Windows/"+self.game_name+".exe"])
                else:
                    print("[YOYO BUILD] Error: I have not supported building games not on linux... so you should not be seeing this message at all.")
            elif(self.game_platform == "emscripten"):
                subprocess.Popen(["emrun", "./bin/Emscripten/index.html"])

    
if __name__ == "__main__":
    builder = YoyoEngineBuildSystem()

    print("----------------------------------")
    print("Yoyo Engine Build Script " + builder.script_version)
    print("Ryan Zmuda 2023")
    print("----------------------------------")
    print("Game Name: " + builder.game_name)
    print("Game Platform: " + builder.game_platform)
    print("Build C Flags: " + builder.build_cflags)
    print("----------------------------------")
    
    if not builder.runonly_flag:
        if(builder.reconfigure == True):
            builder.configure()
        else:
            os.chdir("./build/out")

        builder.build()
        builder.run()
    else:
        builder.run_flag = True

        # chdir into the build folder
        os.chdir("./build/out")

        builder.run()

    print("----------------------------------")
    print("\033[92m" + "Build Successful!" + "\033[0m")
    print("----------------------------------")