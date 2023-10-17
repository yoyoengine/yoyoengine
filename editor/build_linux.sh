#!/bin/bash

# Check if the script was called with the "--preserve" argument
if [ "$1" == "--preserve" ]; then
    # Preserve the existing build directory
    echo "Preserving existing build directory..."
else
    # Delete the build directory if it exists
    if [ -d "./build" ]; then
        echo "Deleting existing build directory..."
        rm -r "./build"
    fi
    # Create a new build directory
    echo "Creating a new build directory..."
    mkdir "./build"
fi

# Navigate into the build directory
cd "./build"

# Run cmake to configure the project
echo "Running cmake..."
cmake ..

# Run make to build the project
echo "Running make to build the project..."
make

# Completion message
echo "Build completed. You can find the built files in the 'build' directory."