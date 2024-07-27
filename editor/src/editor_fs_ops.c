/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <yoyoengine/yoyoengine.h>

#include "editor_fs_ops.h"

/*
    This will be a (for now) platform specific implementation of certain fs ops.
    TODO: NOTCROSSPLATFORM
*/

// recurse copy a directory
bool editor_copy_directory(const char *src, const char *dst) {
    DIR *dir = opendir(src);
    if (!dir) {
        ye_logf(error,"Failed to copy directory directory, could not open: %s\n", src);
        return false;
    }

    if (mkdir(dst, 0755) && errno != EEXIST) {
        ye_logf(error,"Failed to copy directory directory, could not create: %s\n", dst);
        closedir(dir);
        return false;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char src_path[PATH_MAX];
        char dst_path[PATH_MAX];
        snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);
        snprintf(dst_path, sizeof(dst_path), "%s/%s", dst, entry->d_name);

        struct stat statbuf;
        if (stat(src_path, &statbuf) == -1) {
            ye_logf(error,"Failed to copy directory directory, could not stat: %s\n", src_path);
            closedir(dir);
            return false;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            if (!editor_copy_directory(src_path, dst_path)) {
                closedir(dir);
                return false;
            }
        } else {
            int src_fd = open(src_path, O_RDONLY);
            if (src_fd == -1) {
                ye_logf(error,"Failed to copy directory directory, could not open src fd: %s\n", src_path);
                closedir(dir);
                return false;
            }

            int dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, statbuf.st_mode);
            if (dst_fd == -1) {
                ye_logf(error,"Failed to copy directory directory, could not open dst fd: %s\n", dst_path);
                close(src_fd);
                closedir(dir);
                return false;
            }

            char buffer[4096];
            ssize_t bytes;
            while ((bytes = read(src_fd, buffer, sizeof(buffer))) > 0) {
                if (write(dst_fd, buffer, bytes) != bytes) {
                    ye_logf(error,"Failed to copy directory directory, could not write: %s\n", dst_path);
                    close(src_fd);
                    close(dst_fd);
                    closedir(dir);
                    return false;
                }
            }

            close(src_fd);
            close(dst_fd);
        }
    }

    closedir(dir);
    return true;
}

// rename a directory or file
bool editor_rename(const char *src, const char *dst) {
    if (rename(src, dst) == -1) {
        perror("rename");
        return false;
    }
    return true;
}

// only works one level deep of new directories
bool editor_create_directory(const char *path) {
    if (mkdir(path, 0755) && errno != EEXIST) {
        ye_logf(error,"Failed to create directory, could not create: %s\n", path);
        return false;
    }
    return true;
}

void _restore() {
    SDL_WaitEvent(NULL);
    SDL_RestoreWindow(YE_STATE.runtime.window);

    // restore the window to being active (this is not working currently)
    SDL_WaitEvent(NULL);
    SDL_RaiseWindow(YE_STATE.runtime.window);
}

char * editor_file_dialog_select_folder() {
    SDL_MinimizeWindow(YE_STATE.runtime.window);

    // Check if zenity is installed
    if (system("zenity --version") != 0) {
        fprintf(stderr, "Failed to open folder selection dialog, zenity is not installed.\n");
        _restore();
        return NULL;
    }

    char buffer[1024];
    FILE *fp;

    // Run zenity to open folder selection dialog
    fp = popen("zenity --file-selection --directory", "r");
    if (fp == NULL) {
        perror("popen");
        _restore();
        return NULL;
    }

    // Read the selected folder path from zenity
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // Remove trailing newline character
        buffer[strcspn(buffer, "\n")] = 0;
        printf("Selected folder: %s\n", buffer);
    } else {
        printf("No folder selected.\n");
        pclose(fp);
        _restore();
        return NULL;
    }

    pclose(fp);

    _restore();

    return strdup(buffer);
}