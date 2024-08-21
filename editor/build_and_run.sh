#!/usr/bin/env bash

./build_linux.sh "$@"

if [ -z "$1" ]; then
    ./build/bin/Linux/yoyoeditor
else
    if [[ ! $1 = /* ]]; then
        echo "[ERROR] Please provide an absolute path."
        exit 1
    fi

    PROJECT_PATH=$1
    shift

    ./build/bin/Linux/yoyoeditor "$PROJECT_PATH"
fi
