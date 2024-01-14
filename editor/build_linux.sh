#!/bin/bash

# # Run ../engine/build_linux.sh to configure and build the engine
# echo "Running engine build script..."
# cd "../engine"
# ./build_linux.sh
# if [ $? -eq 0 ]
# then
#     echo "Engine build succeeded."
# else
#     echo "Engine build failed."
#     exit 1
# fi
# cd "../editor"

# Create a new build directory
mkdir "./build"

# Navigate into the build directory
cd "./build"

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