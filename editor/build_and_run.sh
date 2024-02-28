#!/bin/bash

if [ -z "$1" ]; then
    echo "Error: No project path provided."
    exit 1
fi

PROJECT_PATH=$1
shift

./build_linux.sh $@
./build/bin/Linux/yoyoeditor "$PROJECT_PATH"