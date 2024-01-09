###############
#    setup    #
###############

cmake_minimum_required(VERSION 3.22.1)
project(yoyoengine)

file(GLOB SOURCES src/*.c src/ecs/*.c src/scripting/*.c CMAKE_CONFIGURE_DEPENDS)

add_library(yoyoengine SHARED ${SOURCES})

target_include_directories(yoyoengine PRIVATE dist/include/)

include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)

###############
#   jansson   #
###############

# Build as shared and without docs - NOTE: jansson doesnt give af about the shared param and builds static anyways
set(BUILD_SHARED 1 CACHE INTERNAL "")
set(JANSSON_BUILD_DOCS OFF CACHE INTERNAL "")
FetchContent_Declare(
    jansson
    GIT_REPOSITORY https://github.com/akheron/jansson.git
    GIT_TAG        v2.14
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(jansson)
target_include_directories(yoyoengine PRIVATE ${jansson_BINARY_DIR}/include)

###############
#    SDL2     #
###############

set(SDL2_DISABLE_INSTALL OFF CACHE BOOL "Disable installation of SDL2") # i forget why this is necessary but im assuming it wont work without
set(SDL_STATIC OFF CACHE BOOL "Build a static library")
set(SDL_SHARED ON CACHE BOOL "Build a shared library")
set(SDL_TEST OFF CACHE BOOL "Build the test programs")
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-2.28.5
    GIT_PROGRESS TRUE
)
# set(SDL_SHARED OFF CACHE BOOL "Build a shared library")
FetchContent_MakeAvailable(SDL2)
target_include_directories(yoyoengine PRIVATE ${SDL2_SOURCE_DIR}/include)

###############
#  SD2L_ttf   #
###############

# let it know it needs to get its vendored deps
set(SDL2TTF_VENDORED ON CACHE BOOL "Use the vendored version of FreeType and HarfBuzz")
FetchContent_Declare(
    SDL2_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf
    GIT_TAG        release-2.20.2
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL2_ttf)
target_include_directories(yoyoengine PRIVATE ${SDL2_ttf_SOURCE_DIR}) # include is in the root of the repo

###############
#  SDL_image  #
###############

FetchContent_Declare(
    SDL2_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG        release-2.8.2
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL2_image)
target_include_directories(yoyoengine PRIVATE ${SDL2_image_SOURCE_DIR}/include)

###############
#  SDL_mixer  #
###############

# disable annoying codecs we dont need that require dependencies i dont have
set(SDL2MIXER_OPUS OFF CACHE BOOL "Enable Opus support")
set(SDL2MIXER_FLAC OFF CACHE BOOL "Enable FLAC support")
set(SDL2MIXER_MOD OFF CACHE BOOL "Enable Mod support")
set(SDL2MIXER_MIDI OFF CACHE BOOL "Enable MIDI support")

# mp3 enabled
set(SDL2MIXER_MP3 ON CACHE BOOL "Enable MP3 support")

# wav enabled
set(SDL2MIXER_WAV ON CACHE BOOL "Enable WAV support")

FetchContent_Declare(
    SDL2_mixer
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG        release-2.6.3
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL2_mixer)
target_include_directories(yoyoengine PRIVATE ${SDL2_mixer_SOURCE_DIR}/include)

###############
#     lua     #
###############

# NOTE: linked as static per the reccomendation of the cmake port maintainer

FetchContent_Declare(
    lua
    GIT_REPOSITORY https://github.com/walterschell/Lua.git
    GIT_TAG        v5.4.5
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(lua)
target_include_directories(yoyoengine PRIVATE ${lua_SOURCE_DIR}/include)

###############
#   linking   #
###############

target_link_libraries(yoyoengine PRIVATE jansson SDL2 SDL2_ttf SDL2_image SDL2_mixer lua_static)

# create an output dir based on platform
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/lib)

# set the output directory for the executable
set_target_properties(yoyoengine PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/lib)

###############
#    libs     #
###############

# Set the output directory for libraries
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/lib)

# Copy libraries to the output directory after build
add_custom_command(
    TARGET yoyoengine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LIBRARY_OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SDL2> ${LIBRARY_OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SDL2_ttf> ${LIBRARY_OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SDL2_image> ${LIBRARY_OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SDL2_mixer> ${LIBRARY_OUTPUT_PATH}
    COMMENT "Copying libraries to platform-specific directory"
)

###############
#   headers   #
###############

set(INCLUDE_FOLDER ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/include)
file(MAKE_DIRECTORY ${INCLUDE_FOLDER})

add_custom_command(
    TARGET yoyoengine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${INCLUDE_FOLDER}/jansson
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${jansson_BINARY_DIR}/include ${INCLUDE_FOLDER}/jansson
    COMMAND ${CMAKE_COMMAND} -E make_directory ${INCLUDE_FOLDER}/SDL2
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SDL2_BINARY_DIR}/include/SDL2 ${INCLUDE_FOLDER}/SDL2
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SDL2_image_SOURCE_DIR}/include ${INCLUDE_FOLDER}/SDL2
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SDL2_mixer_SOURCE_DIR}/include ${INCLUDE_FOLDER}/SDL2
    COMMAND ${CMAKE_COMMAND} -E make_directory ${INCLUDE_FOLDER}/lua
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${lua_SOURCE_DIR}/lua-5.4.5/include ${INCLUDE_FOLDER}/lua

    COMMAND ${CMAKE_COMMAND} -E make_directory ${INCLUDE_FOLDER}/yoyoengine
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/dist/include/yoyoengine ${INCLUDE_FOLDER}/yoyoengine
    
    COMMAND ${CMAKE_COMMAND} -E make_directory ${INCLUDE_FOLDER}/Nuklear
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/dist/include/Nuklear ${INCLUDE_FOLDER}/Nuklear

    COMMAND ${CMAKE_COMMAND} -E make_directory ${INCLUDE_FOLDER}/uthash
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/dist/include/uthash ${INCLUDE_FOLDER}/uthash

    # copy an additional minimal config that for some reason SDL wont generate itself
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/dist/include/SDL2 ${INCLUDE_FOLDER}/SDL2

    COMMENT "Copying header files files to platform-specific directory"
)

###############
#  resources  #
###############

add_custom_command(
    TARGET yoyoengine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/engine_resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/dist/assets ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/engine_resources

    COMMENT "Copying engine resources"
)