# Set the minimum CMake version required
cmake_minimum_required(VERSION 3.27.2)

# Define the project name
project(YoyoEngine)

# Set C/C++ compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

# Define the source files for your project (located in src directory)
file(GLOB SOURCES "src/*.c")

# Include directories for headers
include_directories(dist/include)

# Create a shared library for Linux (.so)
if(UNIX)
    # Define the output shared library name
    set(LIBRARY_NAME yoyoengine)

    # Define the output directory for the shared library
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/linux/lib)

    # Add your source files to the library
    add_library(${LIBRARY_NAME} SHARED ${SOURCES})

    # Copy the contents of /include to the /linux build output
    file(COPY ${CMAKE_SOURCE_DIR}/dist/include DESTINATION ${CMAKE_BINARY_DIR}/linux)

    # Create a /lib directory in the /linux build output and copy platform-specific libraries
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/linux/lib)
    file(GLOB PLATFORM_LIBS ${CMAKE_SOURCE_DIR}/dist/linux/*)
    file(COPY ${PLATFORM_LIBS} DESTINATION ${CMAKE_BINARY_DIR}/linux/lib)

    # copy over engine resources
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/linux/engine_resources)
    file(GLOB ENGINE_RESOURCES ${CMAKE_SOURCE_DIR}/dist/assets/*)
    file(COPY ${ENGINE_RESOURCES} DESTINATION ${CMAKE_BINARY_DIR}/linux/engine_resources)

    # Create a dynamic link library for Windows (.dll)
elseif(WIN32)
    # Define the output shared library name
    set(LIBRARY_NAME yoyoengine)

    # Define the output directory for the shared library
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/windows/lib)

    # Add your source files to the library
    add_library(${LIBRARY_NAME} SHARED ${SOURCES})

    # Copy the contents of /include to the /linux build output
    file(COPY ${CMAKE_SOURCE_DIR}/dist/include DESTINATION ${CMAKE_BINARY_DIR}/windows)

    # Create a /lib directory in the /linux build output and copy platform-specific libraries
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/windows/lib)
    file(GLOB PLATFORM_LIBS ${CMAKE_SOURCE_DIR}/dist/windows/*)
    file(COPY ${PLATFORM_LIBS} DESTINATION ${CMAKE_BINARY_DIR}/windows/lib)
endif()
