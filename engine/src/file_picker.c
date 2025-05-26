/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <yoyoengine/yoyoengine.h>

static void SDLCALL _picker_wrapper(void* userdata, const char* const* filelist, int filter){
    struct ye_picker_data *picker_data = (struct ye_picker_data*)userdata;
    if(!picker_data) {
        ye_logf(YE_LL_ERROR, "Couldn't convert picker data.\n");
        return;
    }

    if(!filelist) {
        ye_logf(YE_LL_ERROR, "An error occured: %s\n", SDL_GetError());
        return;
    }
    else if (!*filelist) {
        ye_logf(YE_LL_DEBUG, "No file selected in engine selector dialog.\n");
        return;
    }
    ye_logf(YE_LL_DEBUG, "Picker selected: %s\n", *filelist);

    /*
        PTR output mode
    */
    if(picker_data->response_mode == YE_PICKER_WRITE_CHAR_PTR) {
        // if output pointer is valid
        if(picker_data->dest.output_ptr){
            // free the an existing ptr value
            if(*picker_data->dest.output_ptr){
                free(*picker_data->dest.output_ptr);
            }
            // write the selected path to the output pointer
            if(picker_data->_truncate_resource_path){
                // truncate the path to the resource folder
                const char *resources_subpath = strstr(*filelist, "resources/");
                if (resources_subpath) {
                    *picker_data->dest.output_ptr = strdup(resources_subpath + strlen("resources/"));
                } else {
                    *picker_data->dest.output_ptr = strdup(*filelist);
                }
            }
            else{
                *picker_data->dest.output_ptr = strdup(*filelist);
            }
        }
        else {
            ye_logf(YE_LL_ERROR, "File browser return write: output pointer is NULL.\n");
        }
    }

    /*
        BUF output mode
    */
    else if(picker_data->response_mode == YE_PICKER_WRITE_CHAR_BUF) {
        if(picker_data->dest.output_buf.buffer && picker_data->dest.output_buf.size > 0){
            // write the selected path directly to the output buffer
            if(picker_data->_truncate_resource_path){
                // truncate the path to the resource folder
                const char *resources_subpath = strstr(*filelist, "resources/");
                if (resources_subpath) {
                    strncpy(picker_data->dest.output_buf.buffer, resources_subpath + strlen("resources/"), 
                            picker_data->dest.output_buf.size - 1);
                    picker_data->dest.output_buf.buffer[picker_data->dest.output_buf.size - 1] = '\0';
                } else {
                    strncpy(picker_data->dest.output_buf.buffer, *filelist, picker_data->dest.output_buf.size - 1);
                    picker_data->dest.output_buf.buffer[picker_data->dest.output_buf.size - 1] = '\0';
                }
            }
            else{
                strncpy(picker_data->dest.output_buf.buffer, *filelist, picker_data->dest.output_buf.size - 1);
                picker_data->dest.output_buf.buffer[picker_data->dest.output_buf.size - 1] = '\0';
            }
        }
        else {
            ye_logf(YE_LL_ERROR, "File browser return write: output buffer is NULL.\n");
        }
    }

    /*
        CB output mode
    */
    else if(picker_data->response_mode == YE_PICKER_FWD_CB) {
        // call the callback function
        picker_data->dest.callback(picker_data->userdata, filelist, filter);
    }
    else {
        ye_logf(YE_LL_ERROR, "File browser return write: invalid response mode.\n");
    }
    
    (void)filter; // unused

    // cleanup the heap copy of data
    free(picker_data);
}

void ye_pick_file(struct ye_picker_data data) {
    // create a heap copy of data
    struct ye_picker_data *picker_data = malloc(sizeof(struct ye_picker_data));
    if (!picker_data) {
        ye_logf(YE_LL_ERROR, "Failed to allocate memory for picker data\n");
        return;
    }
    memcpy(picker_data, &data, sizeof(struct ye_picker_data));

    SDL_ShowOpenFileDialog(
        _picker_wrapper,
        (void*)picker_data,
        YE_STATE.runtime.window,
        picker_data->filter,
        *picker_data->num_filters,
        picker_data->default_location,
        false
    );
}

void ye_pick_resource_file(struct ye_picker_data data) {
    // create a heap copy of data
    struct ye_picker_data *picker_data = malloc(sizeof(struct ye_picker_data));
    if (!picker_data) {
        ye_logf(YE_LL_ERROR, "Failed to allocate memory for picker data\n");
        return;
    }
    memcpy(picker_data, &data, sizeof(struct ye_picker_data));

    // set the truncate resource path flag
    picker_data->_truncate_resource_path = true;

    picker_data->default_location = ye_path_resources(""); // TODO: engine memory hold this as static so we dont need to call
    // this should be ok, but technically this string can change during picker runtime

    SDL_ShowOpenFileDialog(
        _picker_wrapper,
        (void*)picker_data,
        YE_STATE.runtime.window,
        picker_data->filter,
        *picker_data->num_filters,
        picker_data->default_location,
        false
    );
}

void ye_pick_folder(struct ye_picker_data data){
    // create a heap copy of data
    struct ye_picker_data *picker_data = malloc(sizeof(struct ye_picker_data));
    if (!picker_data) {
        ye_logf(YE_LL_ERROR, "Failed to allocate memory for picker data\n");
        return;
    }
    memcpy(picker_data, &data, sizeof(struct ye_picker_data));

    SDL_ShowOpenFolderDialog(
        _picker_wrapper,
        (void*)picker_data,
        YE_STATE.runtime.window,
        picker_data->default_location,
        false
    );
}

/*
    DEFS
*/

SDL_DialogFileFilter ye_picker_image_filters[] = {
    { "PNG images",  "png" },
    { "JPEG images", "jpg;jpeg" },
    { "All images",  "png;jpg;jpeg" }
};
int ye_picker_num_image_filters = sizeof(ye_picker_image_filters) / sizeof(SDL_DialogFileFilter);

SDL_DialogFileFilter ye_picker_audio_filters[] = {
    { "WAV audio",   "wav" },
    { "MP3 audio",   "mp3" },
    { "All audio",   "wav;mp3" }
};
int ye_picker_num_audio_filters = sizeof(ye_picker_audio_filters) / sizeof(SDL_DialogFileFilter);

SDL_DialogFileFilter ye_picker_script_filters[] = {
    { "Lua script",   "lua" }
};
int ye_picker_num_script_filters = sizeof(ye_picker_script_filters) / sizeof(SDL_DialogFileFilter);

SDL_DialogFileFilter ye_picker_yoyo_filters[] = {
    { "yoyo config",   "yoyo" }
};
int ye_picker_num_yoyo_filters = sizeof(ye_picker_yoyo_filters) / sizeof(SDL_DialogFileFilter);

SDL_DialogFileFilter ye_picker_font_filters[] = {
    { "TrueType fonts",   "ttf" }
};
int ye_picker_num_font_filters = sizeof(ye_picker_font_filters) / sizeof(SDL_DialogFileFilter);

SDL_DialogFileFilter ye_picker_icon_filters[] = {
    { "Icon",   "rc" }
};
int ye_picker_num_icon_filters = sizeof(ye_picker_icon_filters) / sizeof(SDL_DialogFileFilter);

SDL_DialogFileFilter ye_picker_any_filters[] = {
    { "All files",   "*" }
};
int ye_picker_num_any_filters = sizeof(ye_picker_any_filters) / sizeof(SDL_DialogFileFilter);