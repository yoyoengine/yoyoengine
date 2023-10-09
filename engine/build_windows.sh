#!/bin/bash

# Delete the build directory if it exists
if [ -d "./build" ]; then
    echo "Deleting existing build directory..."
    rm -r "./build"
fi

# Create a new build directory
echo "Creating a new build directory..."
mkdir "./build"

## Navigate into the build directory
cd "./build"

# Run cmake to configure the project for Windows
echo "Running cmake for Windows..."
cmake -DCMAKE_TOOLCHAIN_FILE=../windows_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE=ON ..

# Run make to build the Windows project
echo "Running make to build the Windows project..."
make

# Completion message
echo "Windows build completed. You can find the built files in the 'build/windows' directory."
