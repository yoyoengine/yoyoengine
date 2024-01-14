#!/bin/bash

if [ -z "$1" ]; then
    echo "Error: No project path provided."
    exit 1
fi

./build_linux.sh
./build/bin/Linux/yoyoeditor "$1"