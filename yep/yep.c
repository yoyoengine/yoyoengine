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

#include "yep.h"

// holds the reference to the currently open yep file
char* yep_file_path = NULL;
FILE *yep_file = NULL;
uint16_t file_entry_count = 0;
uint16_t file_version_number = 0;

struct yep_pack_list yep_pack_list;

bool _yep_open_file(char *file){
    // if we already have this file open, don't open it again
    if(yep_file_path != NULL && strcmp(yep_file_path, file) == 0){
        return true;
    }

    yep_file = fopen(file, "rb");
    if (yep_file == NULL) {
        perror("Error opening file");
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
        printf("Error: file version number (%d) does not match current version number (%d)\n", file_version_number, YEP_CURRENT_FORMAT_VERSION);
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
bool _yep_seek_header(char *handle, char *name, uint32_t *offset, uint32_t *size, uint8_t *compression_type, uint8_t *data_type){
    // go to the beginning of the header section (3 byte) offset from beginning
    fseek(yep_file, 3, SEEK_SET);

    /*
        Its simplist to just read the whole header into memory (the name is most of it) to
        keep ourselves aligned with the headers list
    */
    for(size_t i = 0; i < file_entry_count; i++){
        printf("Searching for %s\n", handle);

        // 64 bytes - name of the resource
        fread(name, sizeof(char), 64, yep_file);

        printf("Comparing against %s\n", name);

        // 4 bytes - offset of the resource
        fread(offset, sizeof(uint32_t), 1, yep_file);

        // 4 bytes - size of the resource
        fread(size, sizeof(uint32_t), 1, yep_file);

        // 1 byte - compression type
        fread(compression_type, sizeof(uint8_t), 1, yep_file);

        // 1 byte - data type
        fread(data_type, sizeof(uint8_t), 1, yep_file);

        // if the name matches, we found the header
        if(strcmp(handle, name) == 0){
            return true;
        }
    }
    return false;
}

void *yep_extract_data(char *file, char *handle){
    if(!_yep_open_file(file)){
        printf("Error opening file %s\n", file);
        return NULL;
    }

    printf("File: %s\n", yep_file_path);
    printf("    Version number: %d\n", file_version_number);
    printf("    Entry count: %d\n", file_entry_count);

    // setup the data we will seek out of the yep file
    char name[64];
    uint32_t offset;
    uint32_t size;
    uint8_t compression_type;
    uint8_t data_type;

    // try to get our header
    if(!_yep_seek_header(handle, name, &offset, &size, &compression_type, &data_type)){
        printf("Error: could not find resource %s in file %s\n", handle, file);
        return NULL;
    }

    // assuming we didnt fail, we have the header data
    printf("Resource:\n");
    printf("    Name: %s\n", name);
    printf("    Offset: %d\n", offset);
    printf("    Size: %d\n", size);
    printf("    Compression type: %d\n", compression_type);
    printf("    Data type: %d\n", data_type);

    // seek to the offset
    fseek(yep_file, offset, SEEK_SET);

    // read the data
    char *data = malloc(size + 1); // null terminator
    fread(data, sizeof(char), size, yep_file);

    // null terminate the data
    data[size] = '\0';

    // return the data
    return (void*)data; // TODO: decode our data and return it in heap
}

void yep_create_test_file(size_t entries){
    FILE *file = fopen("test.yep", "wb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // write the version number (byte 0-1)
    uint8_t version_number = YEP_CURRENT_FORMAT_VERSION;
    fwrite(&version_number, sizeof(uint8_t), 1, file);

    // write the entry count (byte 2-3)
    uint16_t entry_count = (uint16_t)entries;
    fwrite(&entry_count, sizeof(uint16_t), 1, file);

    // write a bunch of dummy entries
    for(size_t i = 0; i < entries; i++){
        // 64 bytes - name of the resource
        char name[64] = "test";
        fwrite(name, sizeof(char), 64, file);

        // 4 bytes - offset of the resource
        uint32_t offset = 0;
        fwrite(&offset, sizeof(uint32_t), 1, file);

        // 4 bytes - size of the resource
        uint32_t size = 0;
        fwrite(&size, sizeof(uint32_t), 1, file);

        // 1 byte - compression type
        uint8_t compression_type = 1;
        fwrite(&compression_type, sizeof(uint8_t), 1, file);

        // 1 byte - data type
        uint8_t data_type = 1;
        fwrite(&data_type, sizeof(uint8_t), 1, file);

        // increment the entry count
        entry_count++;
    }

    fclose(file);
}

void yep_initialize(){
    printf("Initializing yep subsystem...\n");
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

    printf("Shutting down yep subsystem...\n");
}

/*
    Recursively walk the target pack directory and create a LL of files to be packed
*/
void _ye_walk_directory(char *root_path, char *directory_path){
    printf("Walking directory %s...\n", directory_path);

    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;

    if ((dir = opendir(directory_path)) == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", directory_path, entry->d_name);

        if (stat(full_path, &file_stat) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISREG(file_stat.st_mode)) {
            // Calculate the relative path
            char *relative_path = full_path + strlen(root_path) + 1; // this literally offsets the char * to the beginning by the original input
            // ^^^ the +1 here is to get rid of the / in front of the path

            // if the relative path plus its null terminator is greater than 64 bytes, we reject packing this and alert the user
            if(strlen(relative_path) + 1 > 64){
                printf("Error: file %s has a relative path that is too long to pack into a yep file\n", full_path);
                continue;
            }

            printf("%s\n", relative_path);

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

bool yep_pack_directory(char *directory_path, char *output_name){
    printf("Packing directory %s...\n", directory_path);

    // call walk directory (first arg is root, second is current - this is for recursive relative path knowledge)
    _ye_walk_directory(directory_path,directory_path);

    printf("Built pack list...\n");

    // print out all the LL nodes
    struct yep_header_node *itr = yep_pack_list.head;
    while(itr != NULL){
        printf("    %s\n", itr->name);
        printf("    %s\n", itr->fullpath);
        itr = itr->next;
    }

    printf("%d entries\n", yep_pack_list.entry_count);

    /*
        Now, we know exactly the size of our entry list, so we can write the headers for each
        with zerod data for the rest of the fields other than its name
    */

    // open the output file
    FILE *file = fopen(output_name, "wb");
    if (file == NULL) {
        perror("Error opening file");
        return false;
    }

    // write the version number (byte 0-1)
    uint8_t version_number = YEP_CURRENT_FORMAT_VERSION;
    fwrite(&version_number, sizeof(uint8_t), 1, file);

    // write the entry count (byte 2-3)
    uint16_t entry_count = yep_pack_list.entry_count;
    fwrite(&entry_count, sizeof(uint16_t), 1, file);

    // write the headers
    itr = yep_pack_list.head;
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

        // 1 byte - data type
        uint8_t data_type = 0;
        fwrite(&data_type, sizeof(uint8_t), 1, file);

        printf("Wrote header for %s\n", itr->name);

        itr = itr->next;
    }

    // now we know the size of our header section, and can start writing the actual data
    // entry data starts at 3 (file meta) + entry_count * 74 (64 bytes for name, 4 bytes for offset, 4 bytes for size, 1 byte for compression type, 1 byte for data type)
    uint32_t data_start = 3 + (entry_count * 74);
    uint32_t data_end = data_start;
    int header_start = 3;
    int current_entry = 1;

    // in order, lets seek to header_start, open each file and determine what we want to do with it, and write it
    itr = yep_pack_list.head;
    while(itr != NULL){
        // open the file
        FILE *file_to_write = fopen(itr->fullpath, "rb");
        if (file_to_write == NULL) {
            perror("Error opening file");
            exit(1);
            return false;
        }

        // get the size of the file
        fseek(file_to_write, 0L, SEEK_END);
        uint32_t data_size = ftell(file_to_write);
        fseek(file_to_write, 0L, SEEK_SET);

        printf("%s is %d bytes\n", itr->fullpath, data_size);

        // get the data out of the file
        char *data = malloc(data_size);
        fread(data, sizeof(char), data_size, file_to_write);

        // close the file
        fclose(file_to_write);

        // navigate to the start of where we can write new data
        fseek(file, data_end, SEEK_SET);

        // write that data to the pack file
        printf("Writing at offset %d\n", data_end);
        fwrite(data, sizeof(char), data_size, file);

        // update header with the offset and size
        fseek(file, header_start + ((current_entry-1) * 74) + 64, SEEK_SET); // seek to beginning of this entrys header
        // ^^^ we have already set the name (the first 64 bytes), we need to seek 64 bytes forward to where the offset is

        // write the offset value
        printf("Telling header offset was %d\n", data_end);
        fwrite(&data_end, sizeof(uint32_t), 1, file);

        // write the size value
        fwrite(&data_size, sizeof(uint32_t), 1, file);

        // increment the data end
        data_end += data_size;

        // increment the iterator
        itr = itr->next;

        // increment the current entry
        current_entry++;
    }

    // close the file
    fclose(file);

    return true;
}