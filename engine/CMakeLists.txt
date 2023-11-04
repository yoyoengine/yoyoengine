cmake_minimum_required(VERSION 3.22.1)
project(yoyoengine)

# Source files
file(GLOB SOURCES src/*.c src/ecs/*.c) # TODO: dont want to add EVERYTHING later on, platform specific entries

# Define the library
add_library(yoyoengine SHARED ${SOURCES})

# Specify include directories
target_include_directories(yoyoengine
    PUBLIC 
    ${CMAKE_SOURCE_DIR}/dist/include
)

# identify our engine resources
file(GLOB ENGINE_RESOURCES ${CMAKE_SOURCE_DIR}/dist/assets/*)

# set our platform and specify special output var
if(WIN32)
    set(PLATFORM "windows")
    set_target_properties(yoyoengine PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PLATFORM}/lib
    )
elseif(UNIX)
    set(PLATFORM "linux")
    set_target_properties(yoyoengine PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PLATFORM}/lib
    )
endif()

# Specify library directories
target_link_directories(yoyoengine
    PUBLIC
    ${CMAKE_SOURCE_DIR}/dist/${PLATFORM}
)

# Copy over engine resources
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${PLATFORM}/engine_resources)
file(COPY ${ENGINE_RESOURCES} DESTINATION ${CMAKE_BINARY_DIR}/${PLATFORM}/engine_resources)

# Copy the headers in /include
file(COPY ${CMAKE_SOURCE_DIR}/dist/include DESTINATION ${CMAKE_BINARY_DIR}/${PLATFORM})

# copy platform-specific libraries
file(GLOB PLATFORM_LIBS ${CMAKE_SOURCE_DIR}/dist/${PLATFORM}/*)
file(COPY ${PLATFORM_LIBS} DESTINATION ${CMAKE_BINARY_DIR}/${PLATFORM}/lib)

# link libs
target_link_libraries(yoyoengine
    jansson
    lua
    SDL2
    SDL2_image
    SDL2_mixer
    SDL2_ttf
    m
)