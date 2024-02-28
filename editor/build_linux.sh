#!/bin/bash

# Create a new build directory
mkdir "./build"

# Navigate into the build directory
cd "./build"

# only runs cmake if it recieves --reconfigure
if [ "$1" == "--reconfigure" ]
then
    # Run cmake to configure the project
    echo "Running cmake..."
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    if [ $? -eq 0 ]
    then
        echo "cmake configuration succeeded."
    else
        echo "cmake configuration failed."
        exit 1
    fi
fi

# Run make to build the project
echo "Running make..."
make
if [ $? -eq 0 ]
then
    echo "make build succeeded."
else
    echo "make build failed."
    exit 1
fi