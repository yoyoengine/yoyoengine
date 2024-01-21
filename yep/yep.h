/*
    This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

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

#include <stdio.h>      // files
#include <stdint.h>     // int types
#include <stdbool.h>    // bool type
#include <string.h>     // string functions
#include <stdlib.h>     // malloc

#include <dirent.h>         // directory functions
#include <sys/stat.h>       // - stat
#include <linux/limits.h>   // - PATH_MAX

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
    // 4 bytes - uncompressed size
    // 1 byte - data type
    // repeat for entry count
    // data begins
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
void * yep_extract_data(char *file, char *handle);

/**
 * @brief Packs a given directory into a .yep, based on its dir name
 * 
 * @param directory The directory to pack (no spaces)
 * @param output_name The name of the output file (must include extension)
 * @return true Success
 * @return false Failure
 */
bool yep_pack_directory(char *directory_path, char *output_name);

// extract data will call private functions
// _yep_open_file(char *file); which will open the file into the yep global file pointer
// _yep_close_file(); which will close the file on shutdown

/**
 * @brief Initializes the yep subsystem
 */
void yep_initialize();

/**
 * @brief Shuts down the yep subsystem, closing any opened files
 * 
 * NOTE: this does not free memory that has been allocated by the yep reader,
 * you are still responsible for that.
 */
void yep_shutdown();

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