#!/bin/bash

./build_linux.sh $@

if [ -z "$1" ]; then
    ./build/bin/Linux/yoyoeditor
else
    PROJECT_PATH=$1
    shift

    ./build/bin/Linux/yoyoeditor "$PROJECT_PATH"
fi