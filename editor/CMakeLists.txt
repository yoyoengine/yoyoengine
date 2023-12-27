# Set the minimum CMake version required
cmake_minimum_required(VERSION 3.22.1)

# Define the project name
project(YoyoEditor)

# # Include the engine project
# add_subdirectory(/home/ryan/repos/yoyoengine/engine/CMakeLists.txt)

# Set C/C++ compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

# Define the source files for your project (located in src directory)
file(GLOB SOURCES "src/*.c")
# add the source files from src/ui/*.c
file(GLOB UI "src/ui/*.c")
# add the source files from src/ui/panels/*.c
file(GLOB PANELS "src/ui/panels/*.c")

# Define the location of the engine build directory
set(ENGINE_BUILD_DIR "/home/ryan/repos/yoyoengine/engine/build/linux")

# Include directories for headers
include_directories(include ${ENGINE_BUILD_DIR}/include)  # Add the engine's include directory here

# Create a Linux executable
# Create a Linux executable
if(UNIX)
    # Define the output executable name
    set(EXECUTABLE_NAME yoyoeditor)

    # Define the output directory for the editor build
    set(EDITOR_BUILD_DIR "${CMAKE_BINARY_DIR}/linux")

    # Create the executable
    add_executable(${EXECUTABLE_NAME} ${SOURCES} ${UI} ${PANELS})

    # Get a list of all .so files in the lib directory
    file(GLOB LIB_FILES ${ENGINE_BUILD_DIR}/lib/*.so)

    # Link the engine library and all other .so files to the editor
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE 
        ${LIB_FILES}
        SDL2_mixer
        SDL2_ttf
        SDL2_image
        lua
        jansson
        m
    )

    # Specify that the executable should be placed in the 'linux' directory
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${EDITOR_BUILD_DIR}"
    )

    # Copy over engine files (shared library and assets)
    add_custom_command(
        TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ENGINE_BUILD_DIR}/lib" "${EDITOR_BUILD_DIR}/lib"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ENGINE_BUILD_DIR}/engine_resources" "${EDITOR_BUILD_DIR}/engine_resources"
    )

    # Copy over editor files (settings.yoyo)
    add_custom_command(
        TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/settings.yoyo" "${EDITOR_BUILD_DIR}/settings.yoyo"
    )

    # create a BUILD_DIR/PLATFORM/resources directory
    add_custom_command(
        TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${EDITOR_BUILD_DIR}/resources"
    ) # this is a really dumb hack. The engine, in order to get settings.yoyo, traverses into resources/../settings.yoyo which means this folder needs to exist

else()
    message(FATAL_ERROR "The editor is only supported on Linux. If this is a dealbreaker for you, please open an Issue and I will port it. Probably.")
endif()
