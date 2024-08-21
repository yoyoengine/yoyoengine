#!/usr/bin/env bash

# Check for the --reconfigure flag
FORCE_RECONFIGURE=false
if [ "$1" == "--reconfigure" ]; then
    FORCE_RECONFIGURE=true
fi

# Create a new build directory if it doesn't exist
mkdir -p "./build"

# Navigate into the build directory
cd "./build"

# Check if CMakeCache.txt exists or if reconfigure is forced
if [ ! -f "CMakeCache.txt" ] || [ "$FORCE_RECONFIGURE" == true ]; then
    echo "Running cmake..."
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    if [ $? -eq 0 ]; then
        echo "cmake configuration succeeded."
    else
        echo "cmake configuration failed."
        exit 1
    fi

    # Run make without -j4 on the first build (some kind of race condition)
    echo "Running make (first build)..."
    make
    if [ $? -eq 0 ]; then
        echo "make build succeeded."
    else
        echo "make build failed."
        exit 1
    fi
else
    # Run make with -j4 on subsequent builds
	echo "Running make with -j$(nproc)..."
	make -j$(nproc)
    if [ $? -eq 0 ]; then
        echo "make build succeeded."
    else
        echo "make build failed."
        exit 1
    fi
fi
