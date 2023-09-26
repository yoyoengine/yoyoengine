/*
    This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

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

/*
    Please see json.h for implementation details and warnings as well as best practices
*/

#include <yoyoengine/yoyoengine.h>

json_t* ye_json_read(const char* path)
{
    json_error_t jerror;
    json_t* json = json_load_file(path, 0, &jerror);
    if (!json) {
        ye_logf(error, "failed to read json file %s: %s", path, jerror.text);
        return NULL;
    }
    return json;
}

int ye_json_write(const char* path, json_t* json)
{
    json_error_t jerror;
    int ret = json_dump_file(json, path, 0);
    if (ret != 0) {
        ye_logf(error, "failed to write json file %s: %s", path, jerror.text);
        return -1;
    }
    return 0;
}

void ye_json_log(json_t* json)
{
    char* json_str = json_dumps(json, JSON_INDENT(4));
    ye_logf(debug, "%s", json_str);
    free(json_str);
}

int ye_json_merge(json_t* json1, json_t* json2)
{
    if(json_object_update(json1, json2) != 0) {
        ye_logf(error, "failed to merge json objects");
        return -1;
    }
    return 0;
}

int ye_json_merge_existing(json_t* json1, json_t* json2)
{
    if(json_object_update_existing(json1, json2) != 0) {
        ye_logf(error, "failed to merge json objects");
        return -1;
    }
    return 0;
}

int ye_json_merge_missing(json_t* json1, json_t* json2)
{
    if(json_object_update_missing(json1, json2) != 0) {
        ye_logf(error, "failed to merge json objects");
        return -1;
    }
    return 0;
}

bool ye_json_has_key(json_t* json, const char* key)
{
    return json_object_get(json, key) != NULL;
}

bool ye_json_int(json_t* json, const char* key, int *out)
{
    json_t* val = json_object_get(json, key);
    if (!val) {
        ye_logf(warning, "json object does not have key %s", key);
        return false;
    }
    if (!json_is_integer(val)) {
        ye_logf(warning, "json object key %s is not an integer", key);
        return false;
    }
    *out = json_integer_value(val);
    return true;
}

bool ye_json_double(json_t* json, const char* key, double *out)
{
    json_t* val = json_object_get(json, key);
    if (!val) {
        ye_logf(warning, "json object does not have key %s", key);
        return false;
    }
    if (!json_is_real(val)) {
        ye_logf(warning, "json object key %s is not a double", key);
        return false;
    }
    *out = json_real_value(val);
    return true;
}

bool ye_json_bool(json_t* json, const char* key, bool *out)
{
    json_t* val = json_object_get(json, key);
    if (!val) {
        ye_logf(warning, "json object does not have key %s", key);
        return false;
    }
    if (!json_is_boolean(val)) {
        ye_logf(warning, "json object key %s is not a boolean", key);
        return false;
    }
    *out = json_boolean_value(val);
    return true;
}

bool ye_json_string(json_t* json, const char* key, const char **out)
{
    json_t* val = json_object_get(json, key);
    if (!val) {
        ye_logf(warning, "json object does not have key %s", key);
        return false;
    }
    if (!json_is_string(val)) {
        ye_logf(warning, "json object key %s is not a string", key);
        return false;
    }
    *out = json_string_value(val);
    return true;
}

bool ye_json_object(json_t* json, const char* key, json_t **out)
{
    json_t* val = json_object_get(json, key);
    if (!val) {
        ye_logf(warning, "json object does not have key %s", key);
        return false;
    }
    if (!json_is_object(val)) {
        ye_logf(warning, "json object key %s is not an object", key);
        return false;
    }
    *out = val;
    return true;
}

bool ye_json_array(json_t* json, const char* key, json_t **out)
{
    json_t* val = json_object_get(json, key);
    if (!val) {
        ye_logf(warning, "json object does not have key %s", key);
        return false;
    }
    if (!json_is_array(val)) {
        ye_logf(warning, "json object key %s is not an array", key);
        return false;
    }
    *out = val;
    return true;
}

bool ye_json_arr_int(json_t* json, int index, int *out)
{
    json_t* val = json_array_get(json, index);
    if (!val) {
        ye_logf(warning, "json array does not have index %d", index);
        return false;
    }
    if (!json_is_integer(val)) {
        ye_logf(warning, "json array index %d is not an integer", index);
        return false;
    }
    *out = json_integer_value(val);
    return true;
}

bool ye_json_arr_double(json_t* json, int index, double *out)
{
    json_t* val = json_array_get(json, index);
    if (!val) {
        ye_logf(warning, "json array does not have index %d", index);
        return false;
    }
    if (!json_is_real(val)) {
        ye_logf(warning, "json array index %d is not a double", index);
        return false;
    }
    *out = json_real_value(val);
    return true;
}

bool ye_json_arr_bool(json_t* json, int index, bool *out)
{
    json_t* val = json_array_get(json, index);
    if (!val) {
        ye_logf(warning, "json array does not have index %d", index);
        return false;
    }
    if (!json_is_boolean(val)) {
        ye_logf(warning, "json array index %d is not a boolean", index);
        return false;
    }
    *out = json_boolean_value(val);
    return true;
}

bool ye_json_arr_string(json_t* json, int index, const char **out)
{
    json_t* val = json_array_get(json, index);
    if (!val) {
        ye_logf(warning, "json array does not have index %d", index);
        return false;
    }
    if (!json_is_string(val)) {
        ye_logf(warning, "json array index %d is not a string", index);
        return false;
    }
    *out = json_string_value(val);
    return true;
}

bool ye_json_arr_object(json_t* json, int index, json_t **out)
{
    json_t* val = json_array_get(json, index);
    if (!val) {
        ye_logf(warning, "json array does not have index %d", index);
        return false;
    }
    if (!json_is_object(val)) {
        ye_logf(warning, "json array index %d is not an object", index);
        return false;
    }
    *out = val;
    return true;
}

bool ye_json_arr_array(json_t* json, int index, json_t **out)
{
    json_t* val = json_array_get(json, index);
    if (!val) {
        ye_logf(warning, "json array does not have index %d", index);
        return false;
    }
    if (!json_is_array(val)) {
        ye_logf(warning, "json array index %d is not an array", index);
        return false;
    }
    *out = val;
    return true;
}