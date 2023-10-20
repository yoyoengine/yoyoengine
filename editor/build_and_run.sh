#!/bin/bash

if [ -z "$1" ]; then
    echo "Error: No project path provided."
    exit 1
fi

./../engine/build_linux.sh
./build_linux.sh
./build/linux/yoyoeditor "$1"