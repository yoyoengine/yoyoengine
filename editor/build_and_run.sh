#!/bin/bash

if [ -z "$1" ]; then
    echo "Error: No project path provided."
    exit 1
fi

./../engine/build_linux.sh --preserve
./build_linux.sh --preserve
./build/linux/yoyoeditor "$1"