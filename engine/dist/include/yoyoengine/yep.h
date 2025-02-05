/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YEP_H
#define YEP_H

#include <yoyoengine/export.h>

#include <stdio.h>      // files
#include <stdint.h>     // int types
#include <stdbool.h>    // bool type
#include <string.h>     // string functions
#include <stdlib.h>     // malloc

#include <sys/stat.h>       // - stat

#if defined __linux__
    #include <linux/limits.h>   // - PATH_MAX
    #include <dirent.h>         // directory functions
#elif defined WIN32
    #include <platform/windows/dirent.h>
#elif defined __APPLE__
    #include <limits.h>         // - PATH_MAX
    #include <dirent.h>         // directory functions
#endif

#include <jansson.h> // jansson
#include <SDL_mixer.h>
#include <SDL_ttf.h>

/*
    Details on the file format:

    // file begin
    // 1 byte - version number
    // 2 bytes - entry count
    // header start
    // 64 bytes - name of the resource
    // 4 bytes - offset of the resource
    // 4 bytes - size of the resource
    // 1 byte - compression type
    // 4 bytes - uncompressed size (equal to size if uncompressed)
    // 1 byte - data type
    // repeat for entry count
    // data begins
*/

/*
    TODO: add a byte or two for name length, which saves space and allows for longer paths,
    BUT you wont be able to calculate offsets anymore without pre-parsing
*/

#define YEP_CURRENT_FORMAT_VERSION 1

#define YEP_HEADER_SIZE_BYTES 78

// #define YEP_VERSION_NUMBER_SIZE 1   // uint8_t
// #define YEP_ENTRY_COUNT_SIZE 2      // uint16_t

enum YEP_DATATYPE {
    YEP_DATATYPE_MISC,          // loose files, .yoyo .txt etc
    YEP_DATATYPE_IMAGE,         // dont need to differentiate formats because it will be a pixel array from SDL_Image
    YEP_DATATYPE_PCM,           // raw PCM data from SDL_Mixer
    YEP_DATATYPE_LUA_BYTECODE,  // lua bytecode (DO NOT COMPRESS)
};

enum YEP_COMPRESSION {
    YEP_COMPRESSION_NONE,   // no compression
    YEP_COMPRESSION_ZLIB,   // zlib compression
};

/*
    In regards to file handling, lets just keep the most recent file we have opened open,
    that way we can just close whatever we have open at the end, and if we need to swap files during
    runtime we can just detect this and close it.
*/

/**
 * @brief Searches the yep file
 * 
 * @param file The path to the yep file 
 * @param handle The name of the resource to search for
 * @return void* The data of the resource allocated into the heap (NULL if not found) 
 */
YE_API struct yep_data_info yep_extract_data(const char *file, const char *handle);

/**
 * @brief Packs a given directory into a .yep, if the target directory is newer than the last pack, based on its dir name
 * 
 * @param directory The directory to pack (no spaces)
 * @param output_name The name of the output file (must include extension)
 * @return true Success
 * @return false Failure
 */
YE_API bool yep_pack_directory(char *directory_path, char *output_name);

/**
 * @brief ALWAYS packs a given directory into a .yep, based on its dir name
 * 
 * @param directory The directory to pack (no spaces)
 * @param output_name The name of the output file (must include extension)
 * @return true Success
 * @return false Failure
 */
YE_API bool yep_force_pack_directory(char *directory_path, char *output_name);

// extract data will call private functions
// _yep_open_file(char *file); which will open the file into the yep global file pointer
// _yep_close_file(); which will close the file on shutdown

/**
 * @brief Initializes the yep subsystem
 */
YE_API void yep_initialize();

/**
 * @brief Shuts down the yep subsystem, closing any opened files
 * 
 * NOTE: this does not free memory that has been allocated by the yep reader,
 * you are still responsible for that.
 */
YE_API void yep_shutdown();

struct yep_header_node {
    char *fullpath; // used for easy access to file on second pass
    char name[64];
    uint32_t offset;
    uint32_t size;
    uint8_t compression_type;
    uint8_t data_type;

    struct yep_header_node *next;
};

struct yep_pack_list {
    int entry_count;

    struct yep_header_node *head;
};

/*
    =========================
    |       ENGINE API      |
    =========================
*/

struct yep_data_info {
    void *data;
    size_t size;
};

/**
 * @brief Load an image stored inside of resources.yep
 * 
 * @param handle The key storing the image in the file
 * @return SDL_Surface* The loaded image (NULL if not found)
 * 
 * !!! YOU MUST FREE THE SURFACE YOURSELF WHEN YOU ARE DONE WITH IT !!!
 */
YE_API SDL_Surface * yep_resource_image(const char *handle);

/**
 * @brief Load a json file stored inside of resources.yep
 * 
 * @param handle The key storing the json file in the file
 * @return json_t* The loaded json file (NULL if not found)
 * 
 * !!! You must json_decref the json_t when you are done with it. !!!
 */
YE_API json_t * yep_resource_json(const char *handle);

/**
 * @brief Load a audio file stored inside of resources.yep
 * 
 * @param handle The key storing the audio file in the file
 * @return Mix_Chunk* The loaded audio file (NULL if not found)
 * 
 * !!! YOU MUST FREE THE CHUNK YOURSELF WHEN YOU ARE DONE WITH IT !!!
 */
YE_API Mix_Chunk * yep_resource_audio(const char *handle);

/**
 * @brief Load a music file stored inside of resources.yep
 * 
 * @param handle The key storing the music file in the file
 * @return Mix_Music* The loaded music file (NULL if not found)
 * 
 * !!! YOU MUST FREE THE MUSIC YOURSELF WHEN YOU ARE DONE WITH IT !!!
 */
YE_API Mix_Music * yep_resource_music(const char *handle);

/**
 * @brief Load a font file stored inside of resources.yep
 * 
 * @param handle The key storing the font file in the file
 * @return TTF_Font* The loaded font file (NULL if not found)
 * 
 * !!! YOU MUST FREE THE FONT YOURSELF WHEN YOU ARE DONE WITH IT !!!
 */
YE_API TTF_Font * yep_resource_font(const char *handle);

/**
 * @brief Load a misc file stored inside of resources.yep
 * 
 * @param handle The key storing the misc file in the file
 * @return struct yep_data_info The loaded misc file (NULL if not found)
 * 
 * !!! YOU MUST FREE THE DATA YOURSELF WHEN YOU ARE DONE WITH IT !!!
 */
YE_API struct yep_data_info yep_resource_misc(const char *handle);

/*
    Engine api as well
*/

YE_API SDL_Surface * yep_engine_resource_image(const char *handle);

YE_API json_t * yep_engine_resource_json(const char *handle);

YE_API Mix_Chunk * yep_engine_resource_audio(const char *handle);

YE_API Mix_Music * yep_engine_resource_music(const char *handle);

YE_API TTF_Font * yep_engine_resource_font(const char * handle);

YE_API struct yep_data_info yep_engine_resource_misc(const char *handle);

#endif // YEP_H