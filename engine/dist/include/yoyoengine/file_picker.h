/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef FILE_PICKER_H
#define FILE_PICKER_H

#include <SDL3/SDL.h>

#include <yoyoengine/file_picker.h>

/*
    File picker response wrapper modes.
*/
enum ye_picker_response_mode {
    YE_PICKER_WRITE_CHAR_PTR,
    YE_PICKER_WRITE_CHAR_BUF,
    YE_PICKER_FWD_CB
};

/*
    Wrap SDL_FilePicker parameters with some custom
    response modes.
*/
struct ye_picker_data {
    SDL_DialogFileFilter *filter;
    int *num_filters;
    const char *default_location;
    void *userdata;

    // custom extra
    enum ye_picker_response_mode response_mode;
    union {
        char **output_ptr;
        struct {
            char *buffer;
            size_t size;
        } output_buf;
        SDL_DialogFileCallback callback;
    } dest;

    // private (internal)
    bool _truncate_resource_path;
};

/*
    Pick a single file, use struct args to set parameters.
*/
void ye_pick_file(struct ye_picker_data data);

/*
    Pick a single file, use struct args to set parameters, but do not set
    default_location beforehand.

    Returns the truncated file path relative to the resource folder.
*/
void ye_pick_resource_file(struct ye_picker_data data);

/*
    Pick a folder, use struct args to set parameters.
*/
void ye_pick_folder(struct ye_picker_data data);

/*
    DEFS
*/
extern SDL_DialogFileFilter ye_picker_image_filters[];
extern int ye_picker_num_image_filters;

extern SDL_DialogFileFilter ye_picker_audio_filters[];
extern int ye_picker_num_audio_filters;

extern SDL_DialogFileFilter ye_picker_script_filters[];
extern int ye_picker_num_script_filters; 

extern SDL_DialogFileFilter ye_picker_yoyo_filters[];
extern int ye_picker_num_yoyo_filters; 

extern SDL_DialogFileFilter ye_picker_font_filters[];
extern int ye_picker_num_font_filters; 

extern SDL_DialogFileFilter ye_picker_icon_filters[];
extern int ye_picker_num_icon_filters; 

extern SDL_DialogFileFilter ye_picker_any_filters[];
extern int ye_picker_num_any_filters;

#endif