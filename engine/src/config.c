/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <string.h>
#include <stdbool.h>

#include <yoyoengine/json.h>
#include <yoyoengine/config.h>

/*
    This is a set of reusable functions to read a typed value
    from a json object, and set it to a default if unset
*/

int ye_config_int(json_t* config, const char* key, int default_value) {
    int value;
    if (ye_json_int(config, key, &value)) {
        return value;
    }

    // set the key
    json_t *new = json_integer(default_value);
    json_object_set(config, key, new);
    return default_value;
}

float ye_config_float(json_t* config, const char* key, float default_value) {
    float value;
    if (ye_json_float(config, key, &value)) {
        return value;
    }

    // set the key
    json_t *new = json_real(default_value);
    json_object_set(config, key, new);
    return default_value;
}

char* ye_config_string(json_t* config, const char* key, const char* default_value) {
    const char* value;
    if (ye_json_string(config, key, &value)) {
        return strdup(value);
    }

    // set the key
    json_t *new = json_string(default_value);
    json_object_set(config, key, new);
    return strdup(default_value);
}

bool ye_config_bool(json_t* config, const char* key, bool default_value) {
    bool value;
    if (ye_json_bool(config, key, &value)) {
        return value;
    }

    // set the key
    json_t *new = json_boolean(default_value);
    json_object_set(config, key, new);
    return default_value;
}