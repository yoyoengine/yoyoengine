#!/bin/bash

# Create a new build directory
mkdir "./build"
mkdir "./build/windows"

# Navigate into the build directory
cd "./build/windows"

# Run cmake to configure the project
echo "Running cmake..."
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=toolchains/windows.cmake ../..
if [ $? -eq 0 ]
then
    echo "cmake configuration succeeded."
else
    echo "cmake configuration failed."
    exit 1
fi

# Run make to build the project
echo "Running make..."
make -j4
if [ $? -eq 0 ]
then
    echo "make build succeeded."
else
    echo "make build failed."
    exit 1
fi