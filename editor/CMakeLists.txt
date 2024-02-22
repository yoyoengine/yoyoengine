# Minimum CMake version and project name
cmake_minimum_required(VERSION 3.22.1)
project(YoyoEditor)

# set YOYO_CMAKE_COPY_ENGINE_RESOURCES to true/on since editor needs the engine resources loose folder
set(YOYO_CMAKE_COPY_ENGINE_RESOURCES true CACHE BOOL "Copy the engine resources to the editor build directory")

# Include the engine project and directories for headers
add_subdirectory(../engine engine_build)
include_directories(include ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/include)

# Set C/C++ compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

# Define the source files for your project
file(GLOB_RECURSE SOURCES CMAKE_CONFIGURE_DEPENDS "src/*.c")

# Check if the system is UNIX
if(UNIX)
    # Define the output executable name and directory
    set(EXECUTABLE_NAME yoyoeditor)
    set(EDITOR_BUILD_DIR "${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}")

    # Create the executable and link the engine library
    add_executable(${EXECUTABLE_NAME} ${SOURCES})
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE yoyoengine m)

    # Set the output directory for the executable
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${EDITOR_BUILD_DIR}"
    )

    # Post build commands to copy files and create directories
    add_custom_command(
        TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/settings.yoyo" "${EDITOR_BUILD_DIR}/settings.yoyo"
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/editor.yoyo" "${EDITOR_BUILD_DIR}/editor.yoyo"
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${EDITOR_BUILD_DIR}/resources"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/editor_resources" "${EDITOR_BUILD_DIR}/engine_resources"
    )
else()
    message(FATAL_ERROR "The editor is only supported on Linux. If this is a dealbreaker for you, please open an Issue and I will port it. Probably.")
endif()