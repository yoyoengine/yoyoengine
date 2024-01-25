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

#include <SDL2/SDL_image.h>
#include <SDL_mixer.h>

#include <jansson/jansson.h> // jansson

#include <yoyoengine/yoyoengine.h>
#include <yoyoengine/yep.h>

#include <zlib.h>   // zlib compression

// holds the reference to the currently open yep file
char* yep_file_path = NULL;
FILE *yep_file = NULL;
uint16_t file_entry_count = 0;
uint16_t file_version_number = 0;

struct yep_pack_list yep_pack_list;

/*
    ========================= COMPRESSION IMPLEMENTATION =========================
*/

int compress_data(const char* input, size_t input_size, char** output, size_t* output_size) {
    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    if (deflateInit(&stream, Z_DEFAULT_COMPRESSION) != Z_OK) { // TODO: could add support for the other compression levels
        return -1;
    }

    // Set input data
    stream.next_in = (Bytef*)input;
    stream.avail_in = input_size;

    // Allocate initial output buffer
    *output_size = input_size + input_size / 10 + 12; // Adding some extra space for safety
    *output = (char*)malloc(*output_size);

    // Set output buffer
    stream.next_out = (Bytef*)*output;
    stream.avail_out = *output_size;

    // Compress the data
    if (deflate(&stream, Z_FINISH) != Z_STREAM_END) {
        free(*output);
        deflateEnd(&stream);
        return -1;
    }

    // Clean up
    deflateEnd(&stream);
    *output_size = stream.total_out;

    return 0;
}

int decompress_data(const char* input, size_t input_size, char** output, size_t output_size) {
    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    int inflate_result = inflateInit(&stream);
    if (inflate_result != Z_OK) {
        ye_logf(error, "inflateInit error: %s\n", zError(inflate_result));
        return -1;
    }

    // Set input data
    stream.next_in = (Bytef*)input;
    stream.avail_in = input_size;

    // Allocate initial output buffer
    *output = (char*)malloc(output_size);

    // Set output buffer
    stream.next_out = (Bytef*)*output;
    stream.avail_out = output_size;

    // Decompress the data
    int res = inflate(&stream, Z_FINISH) != Z_STREAM_END;
    if (res) {
        free(*output);
        inflateEnd(&stream);
        ye_logf(error,"Error decompressing data: %s\n",zError(res));
        return -1;
    }

    // Clean up
    inflateEnd(&stream);

    if(output_size != stream.total_out){
        ye_logf(error,"Error: decompressed size does not match expected size\n");
        return -1;
    }

    return 0;
}

/*
    ==============================================================================
*/

// utility function via chatgpt - moveme //

void displayProgressBar(int current, int max) {
    // Calculate the percentage completion
    float progress = (float)current / max;
    
    // Determine the length of the progress bar
    int barLength = 50;
    int progressLength = (int)(progress * barLength);
    
    // Clear the current line
    printf("\r");

    // Display the progress bar
    printf("[");
    for (int i = 0; i < barLength; ++i) {
        if (i < progressLength) {
            printf("=");
        } else {
            printf(" ");
        }
    }
    printf("] %.2f%% (%d/%d)", progress * 100, current, max);
    
    // Flush the output to ensure it's immediately displayed
    fflush(stdout);
}

///////////////////////////////////////////

bool _yep_open_file(char *file){
    // if we already have this file open, don't open it again
    if(yep_file_path != NULL && strcmp(yep_file_path, file) == 0){
        return true;
    }

    yep_file = fopen(file, "rb");
    if (yep_file == NULL) {
        ye_logf(error,"Error opening yep file\n");
        return false;
    }

    // set the file path
    if(yep_file_path != NULL)
        free(yep_file_path);

    yep_file_path = strdup(file);

    // read the version number (byte 0-1)
    fread(&file_version_number, sizeof(uint8_t), 1, yep_file);

    // read the entry count (byte 2-3)
    fread(&file_entry_count, sizeof(uint16_t), 1, yep_file);

    if(file_version_number != YEP_CURRENT_FORMAT_VERSION){
        ye_logf(error,"Error: file version number (%d) does not match current version number (%d)\n", file_version_number, YEP_CURRENT_FORMAT_VERSION);
        return false;
    }

    return true;
}

void _yep_close_file(){
    if(yep_file != NULL){
        fclose(yep_file);
        yep_file = NULL;

        if(yep_file_path != NULL)
            free(yep_file_path);

        file_entry_count = 0;
        file_version_number = 0;
    }
}

/*
    Takes in references to where to output the data if found, and returns true if found, false if not found
*/
bool _yep_seek_header(char *handle, char *name, uint32_t *offset, uint32_t *size, uint8_t *compression_type, uint32_t *uncompressed_size, uint8_t *data_type){
    // go to the beginning of the header section (3 byte) offset from beginning
    fseek(yep_file, 3, SEEK_SET);

    /*
        Its simplist to just read the whole header into memory (the name is most of it) to
        keep ourselves aligned with the headers list
    */
    for(size_t i = 0; i < file_entry_count; i++){
        // printf("Searching for %s\n", handle);

        // 64 bytes - name of the resource
        fread(name, sizeof(char), 64, yep_file);

        // printf("Comparing against %s\n", name);

        // 4 bytes - offset of the resource
        fread(offset, sizeof(uint32_t), 1, yep_file);

        // 4 bytes - size of the resource
        fread(size, sizeof(uint32_t), 1, yep_file);

        // 1 byte - compression type
        fread(compression_type, sizeof(uint8_t), 1, yep_file);

        // 4 bytes - uncompressed size
        fread(uncompressed_size, sizeof(uint32_t), 1, yep_file);

        // 1 byte - data type
        fread(data_type, sizeof(uint8_t), 1, yep_file);

        // if the name matches, we found the header
        if(strcmp(handle, name) == 0){
            return true;
        }
    }
    return false;
}

struct yep_data_info yep_extract_data(char *file, char *handle){
    if(!_yep_open_file(file)){
        ye_logf(error,"Error opening yep file %s\n", file);
        exit(1);
    }

    // printf("File: %s\n", yep_file_path);
    // printf("    Version number: %d\n", file_version_number);
    // printf("    Entry count: %d\n", file_entry_count);

    // setup the data we will seek out of the yep file
    char name[64];
    uint32_t offset;
    uint32_t size;
    uint8_t compression_type;
    uint32_t uncompressed_size;
    uint8_t data_type;

    // try to get our header
    if(!_yep_seek_header(handle, name, &offset, &size, &compression_type, &uncompressed_size, &data_type)){
        ye_logf(error,"Error: could not find resource %s in file %s\n", handle, file);
        exit(1);
    }

    // assuming we didnt fail, we have the header data
    // printf("Resource:\n");
    // printf("    Name: %s\n", name);
    // printf("    Offset: %d\n", offset);
    // printf("    Size: %d\n", size);
    // printf("    Uncompressed size: %d\n", uncompressed_size);
    // printf("    Compression type: %d\n", compression_type);
    // printf("    Data type: %d\n", data_type);

    // seek to the offset
    fseek(yep_file, offset, SEEK_SET);

    // read the data
    char *data = malloc(size + 1); // null terminator
    fread(data, sizeof(char), size, yep_file);

    // null terminate the data
    if(compression_type == YEP_COMPRESSION_NONE)
        data[size] = '\0';

    // printf("DATA VALUE LITERAL: %s\n", data);

    // if the data is compressed, decompress it
    if(compression_type == YEP_COMPRESSION_ZLIB){
        char *decompressed_data;
        if(decompress_data(data, size, &decompressed_data, uncompressed_size) != 0){
            ye_logf(error,"!!!Error decompressing data!!!\n");
            exit(1);
        }

        // printf("Decompressed %s from %d bytes to %d bytes\n", handle, size, uncompressed_size);
        // printf("    Compression ratio: %f\n", (float)uncompressed_size / (float)size);
        // printf("    Compression percentage: %f%%\n", ((float)uncompressed_size / (float)size) * 100.0f);
        // printf("    Compression savings: %d bytes\n", uncompressed_size - size);
        // printf("    DATA: %s\n", decompressed_data);

        // free the original data
        free(data);

        // set the data to the decompressed data
        data = decompressed_data;
        size = uncompressed_size;
    }

    // create return data
    struct yep_data_info info;
    info.data = data;
    info.size = size;

    // return the data
    return info;
}

void yep_initialize(){
    ye_logf(info,"Initializing yep subsystem...\n");
    yep_pack_list.entry_count = 0;
}

void yep_shutdown(){
    _yep_close_file();

    if(yep_pack_list.head != NULL){
        struct yep_header_node *itr = yep_pack_list.head;
        while(itr != NULL){
            struct yep_header_node *next = itr->next;
            free(itr->fullpath);
            free(itr);
            itr = next;
        }
    }

    ye_logf(info,"Shutting down yep subsystem...\n");
}


/*
    Isolate the platform specific packing functionality that is only needed
    when building games (which should be done on linux if youre a real developer)
*/
#ifdef __linux__

/*
    Recursively walk the target pack directory and create a LL of files to be packed
*/
void _ye_walk_directory(char *root_path, char *directory_path){
    // printf("Walking directory %s...\n", directory_path);

    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;

    if ((dir = opendir(directory_path)) == NULL) {
        ye_logf(error,"Could not open directory %s\n", directory_path);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", directory_path, entry->d_name);

        if (stat(full_path, &file_stat) == -1) {
            ye_logf(error,"Could not stat %s\n", full_path);
            continue;
        }

        if (S_ISREG(file_stat.st_mode)) {
            // Calculate the relative path
            char *relative_path = full_path + strlen(root_path) + 1; // this literally offsets the char * to the beginning by the original input
            // ^^^ the +1 here is to get rid of the / in front of the path

            // if the relative path plus its null terminator is greater than 64 bytes, we reject packing this and alert the user
            if(strlen(relative_path) + 1 > 64){
                ye_logf(error,"Error: file %s has a relative path that is too long to pack into a yep file\n", full_path);
                continue;
            }

            // printf("%s\n", relative_path);

            // add a yep header node with the relative path
            struct yep_header_node *node = malloc(sizeof(struct yep_header_node));

            // set the name field to zeros so I dont lose my mind reading hex output
            memset(node->name, 0, 64);

            // set the full path
            node->fullpath = strdup(full_path);

            // set the name
            sprintf(node->name, "%s", relative_path);
            node->name[strlen(relative_path)] = '\0'; // ensure null termination

            // add the node to the LL
            node->next = yep_pack_list.head;
            yep_pack_list.head = node;

            // increment the entry count
            yep_pack_list.entry_count++;
        }
        else if (S_ISDIR(file_stat.st_mode)) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                _ye_walk_directory(root_path, full_path);
            }
        }
    }

    closedir(dir);
}

/*
    Returns the size of a file in bytes
*/
uint32_t get_file_size(FILE *file) {
    fseek(file, 0L, SEEK_END);
    uint32_t size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return size;
}

/*
    Reads a full file into memory and returns a pointer to it

    Assumes the file is open and seeked to the beginning
*/
char* read_file_data(FILE *file, uint32_t size) {
    char *data = malloc(size);
    fread(data, sizeof(char), size, file);
    return data;
}

/*
    Writes data to a pack file at a given offset
*/
void write_data_to_pack(FILE *pack_file, uint32_t offset, char *data, uint32_t size) {
    fseek(pack_file, offset, SEEK_SET);
    fwrite(data, sizeof(char), size, pack_file);
}

/*
    Updates a pack file header with details of data just written
*/
void update_header(FILE *pack_file, int entry_index, uint32_t offset, uint32_t size, uint8_t compression_type, uint32_t uncompressed_size, uint8_t data_type) {
    int header_start = 3;
    
    // get where this specific header starts, and move to its offset field (name is already set)
    int header_offset = header_start + (entry_index * YEP_HEADER_SIZE_BYTES) + 64;
    fseek(pack_file, header_offset, SEEK_SET);

    // write the data offset and data size
    fwrite(&offset, sizeof(uint32_t), 1, pack_file);
    fwrite(&size, sizeof(uint32_t), 1, pack_file);

    // write the compression type, uncompressed size and data type
    fwrite(&compression_type, sizeof(uint8_t), 1, pack_file);
    fwrite(&uncompressed_size, sizeof(uint32_t), 1, pack_file);
    fwrite(&data_type, sizeof(uint8_t), 1, pack_file);
}

void write_pack_file(FILE *pack_file) {
    // holds the start of the header for our current entry
    uint32_t data_start = 3 + (yep_pack_list.entry_count * YEP_HEADER_SIZE_BYTES);

    // holds the end of the data pack
    uint32_t data_end = data_start;

    // holds the current entry
    int current_entry = 0;

    printf("\n"); // start the progress bar on a new line

    struct yep_header_node *itr = yep_pack_list.head;
    while(itr != NULL){

        FILE *file_to_write = fopen(itr->fullpath, "rb");
        if (file_to_write == NULL) {
            ye_logf(error,"Error opening yep file to pack yep: %s\n", itr->fullpath);
            exit(1);
        }

        uint32_t data_size = get_file_size(file_to_write);
        uint32_t uncompressed_size = data_size;
        char *data = read_file_data(file_to_write, data_size);
        fclose(file_to_write);

        // somewhere here is where we would perform our compression or
        // manipulation of the data depending on its format
        uint8_t compression_type = (uint8_t)YEP_COMPRESSION_NONE;
        uint8_t data_type = (uint8_t)YEP_DATATYPE_MISC;

        if(
            data_size > 256
            // here is where we can && exclusion conditions, like bytecode
        ){
            compression_type = (uint8_t)YEP_COMPRESSION_ZLIB;
        }

        // compress this data with zlib
        if(compression_type == YEP_COMPRESSION_ZLIB){
            char *compressed_data;
            size_t compressed_size;
            compress_data(data, data_size, &compressed_data, &compressed_size);

            // printf("Compressed %s from %d bytes to %d bytes\n", itr->fullpath, data_size, compressed_size);
            // printf("    Compression ratio: %f\n", (float)compressed_size / (float)data_size);
            // printf("    Compression percentage: %f%%\n", ((float)compressed_size / (float)data_size) * 100.0f);
            // printf("    Compression savings: %d bytes\n", data_size - compressed_size);
            // printf("    DATA: %s\n", compressed_data);

            // free the original data
            free(data);

            // set the data to the compressed data
            data = compressed_data;
            data_size = compressed_size;
        }

        // write the actual data from our data file to the pack file
        write_data_to_pack(pack_file, data_end, data, data_size);

        // update the pack file header with the location and information about the data we wrote
        update_header(pack_file, current_entry, data_end, data_size, compression_type, uncompressed_size, data_type);

        // free the data
        free(data);

        // shift the end pointer of the data pack file
        data_end += data_size;

        // incr
        itr = itr->next;
        current_entry++;

        displayProgressBar(current_entry, yep_pack_list.entry_count);
    }
    printf("\n\n"); // let next log start on new line
    fclose(pack_file);

    // clean up global pack list and variables
    struct yep_header_node *itr2 = yep_pack_list.head;
    while(itr2 != NULL){
        struct yep_header_node *next = itr2->next;
        free(itr2->fullpath);
        free(itr2);
        itr2 = next;
    }
    yep_pack_list.head = NULL;
    yep_pack_list.entry_count = 0;
}

bool yep_pack_directory(char *directory_path, char *output_name){
    ye_logf(debug,"Packing directory %s...\n", directory_path);

    // call walk directory (first arg is root, second is current - this is for recursive relative path knowledge)
    _ye_walk_directory(directory_path,directory_path);

    ye_logf(debug,"Built pack list...\n");

    // print out all the LL nodes
    // struct yep_header_node *itr = yep_pack_list.head;
    // while(itr != NULL){
        // printf("    %s\n", itr->name);
        // printf("    %s\n", itr->fullpath);
        // itr = itr->next;
    // }

    ye_logf(debug,"Detected %d entries\n", yep_pack_list.entry_count);

    /*
        Now, we know exactly the size of our entry list, so we can write the headers for each
        with zerod data for the rest of the fields other than its name
    */

    // open the output file
    FILE *file = fopen(output_name, "wb");
    if (file == NULL) {
        ye_logf(error,"Error opening yep file %s\n", output_name);
        return false;
    }

    // write the version number (byte 0-1)
    uint8_t version_number = YEP_CURRENT_FORMAT_VERSION;
    fwrite(&version_number, sizeof(uint8_t), 1, file);

    // write the entry count (byte 2-3)
    uint16_t entry_count = yep_pack_list.entry_count;
    fwrite(&entry_count, sizeof(uint16_t), 1, file);

    ye_logf(debug,"Writing headers...\n");

    // write the headers
    struct yep_header_node *itr = yep_pack_list.head;
    while(itr != NULL){
        // 64 bytes - name of the resource
        fwrite(itr->name, sizeof(char), 64, file);

        // 4 bytes - offset of the resource
        uint32_t offset = 0;
        fwrite(&offset, sizeof(uint32_t), 1, file);

        // 4 bytes - size of the resource
        uint32_t size = 0;
        fwrite(&size, sizeof(uint32_t), 1, file);

        // 1 byte - compression type
        uint8_t compression_type = 0;
        fwrite(&compression_type, sizeof(uint8_t), 1, file);

        // 4 bytes - uncompressed size
        uint32_t uncompressed_size = 0;
        fwrite(&uncompressed_size, sizeof(uint32_t), 1, file);

        // 1 byte - data type
        uint8_t data_type = 0;
        fwrite(&data_type, sizeof(uint8_t), 1, file);

        // printf("Wrote header for %s\n", itr->name);

        itr = itr->next;
    }

    ye_logf(debug,"Writing data...\n");

    // write the data
    write_pack_file(file);

    ye_logf(debug,"Done!\n");

    return true;
}

#endif

/*
    YEP TODO:
    - native animation functionality (this will cutout a lot of headers)
    - actually hook an API so engine can get certain types
    - encode the RGBA and PCM data rather than the file binary
*/

/*
    ENGINE API
*/

/*
    Backend impl that takes in full details
*/

struct yep_data_info _yep_misc(char *handle, char *file){
    // get and validate the data
    struct yep_data_info data = yep_extract_data(file, handle);
    if(data.data == NULL){
        ye_logf(error,"Error: could not get misc data for %s\n", handle);
        return data;
    }

    // return the data
    return data;
}

SDL_Surface * _yep_image(char *handle, char *path){
    // load the data
    struct yep_data_info data = _yep_misc(handle, path);

    // create the surface
    SDL_Surface *surface = IMG_Load_RW(SDL_RWFromMem(data.data, data.size), 1);
    if(surface == NULL){
        ye_logf(error,"Error: could not create surface for %s\n", handle);
        return NULL;
    }

    // free the data
    free(data.data);

    // return the surface
    return surface;
}

json_t * _yep_json(char *handle, char *path){
    // load the data
    struct yep_data_info data = _yep_misc(handle, path);

    // create the json
    json_t *json = json_loadb(data.data, data.size, 0, NULL);
    if(json == NULL){
        ye_logf(error,"Error: could not create json for %s\n", handle);
        return NULL;
    }

    // free the data
    free(data.data); // PAST RYAN TO FUTURE RYAN: YOU MIGHT NOT HAVE TO FREE THIS BECAUSE
    // SDL TAKES OWNERSHIP I THINK, YOU NEED TO LOOK IT UP

    // return the json
    return json;
}

Mix_Chunk * _yep_audio(char *handle, char *path){
    // load the data
    struct yep_data_info data = _yep_misc(handle, path);

    // create the chunk
    Mix_Chunk *chunk = Mix_LoadWAV_RW(SDL_RWFromMem(data.data, data.size), 1);
    if(chunk == NULL){
        ye_logf(error,"Error: could not create chunk for %s\n", handle);
        return NULL;
    }

    // free the data
    free(data.data);

    // return the chunk
    return chunk;
}

TTF_Font * _yep_font(char *handle, char *path){
    // load the data
    struct yep_data_info data = _yep_misc(handle, path);

    // create the font
    TTF_Font *font = TTF_OpenFontRW(SDL_RWFromMem(data.data, data.size), 1, 1);
    if(font == NULL){
        ye_logf(error,"Error: could not create font for %s\n", handle);
        return YE_STATE.engine.pEngineFont;
    }

    // free the data
    // free(data.data);
    // DO NOT free the data because SDL_RWFromMem takes ownership of the data
    // at least I think... you should valgrind it because it wont work if you free here

    // return the font
    return font;
}

/*
    Accessor functions that abstract the file they come from
*/

SDL_Surface * yep_resource_image(char *handle){
    return _yep_image(handle, "resources.yep");
}

json_t * yep_resource_json(char *handle){
    return _yep_json(handle, "resources.yep");
}

Mix_Chunk * yep_resource_audio(char *handle){
    return _yep_audio(handle, "resources.yep");
}

TTF_Font * yep_resource_font(char * handle){
    return _yep_font(handle, "resources.yep");
}

struct yep_data_info yep_resource_misc(char *handle){
    return _yep_misc(handle, "resources.yep");
}

/*
    Just for ease of use (who cares about LOC) lets provide some accessors for engine resources
*/

SDL_Surface * yep_engine_resource_image(char *handle){
    /*
        If in editor mode, we need to load from loose file
    */
    if(YE_STATE.editor.editor_mode){
        SDL_Surface *surface = IMG_Load(ye_get_engine_resource_static(handle));
        if(surface == NULL){
            ye_logf(error,"Error: could not create surface for %s\n", handle);
            return NULL;
        }
        return surface;
    }
    else{
        return _yep_image(handle, ye_path("engine.yep"));
    }
}

json_t * yep_engine_resource_json(char *handle){
    /*
        If in editor mode, we need to load from loose file
    */
    if(YE_STATE.editor.editor_mode){
        json_t *json = json_load_file(ye_get_engine_resource_static(handle), 0, NULL);
        if(json == NULL){
            ye_logf(error,"Error: could not create json for %s\n", handle);
            return NULL;
        }
        return json;
    }
    else{
        return _yep_json(handle, ye_path("engine.yep"));
    }
}

Mix_Chunk * yep_engine_resource_audio(char *handle){
    /*
        If in editor mode, we need to load from loose file
    */
    if(YE_STATE.editor.editor_mode){
        Mix_Chunk *chunk = Mix_LoadWAV(ye_get_engine_resource_static(handle));
        if(chunk == NULL){
            ye_logf(error,"Error: could not create chunk for %s\n", handle);
            return NULL;
        }
        return chunk;
    }
    else{
        return _yep_audio(handle, ye_path("engine.yep"));
    }
}

TTF_Font * yep_engine_resource_font(char * handle){
    /*
        If in editor mode, we need to load from loose file
    */
    if(YE_STATE.editor.editor_mode){
        TTF_Font *font = TTF_OpenFont(ye_get_engine_resource_static(handle), 20);
        if(font == NULL){
            ye_logf(error,"Error: could not create font for %s\n", handle);
            return NULL;
        }
        return font;
    }
    else{
        return _yep_font(handle, ye_path("engine.yep"));
    }
}

struct yep_data_info yep_engine_resource_misc(char *handle){
    return _yep_misc(handle, ye_path("engine.yep"));
}