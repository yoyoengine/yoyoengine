#!/bin/bash

# This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
# Copyright (C) 2024  Ryan Zmuda
# Licensed under the MIT license. See LICENSE file in the project root for details.

# Check dependencies
dependencies=("curl" "tar")
for dependency in "${dependencies[@]}"; do
    if ! command -v "$dependency" &> /dev/null; then
        echo "Error: $dependency is not installed."
        exit 1
    fi
done

# testing:
# release_tar_url=$(curl -s https://api.github.com/repos/zoogies/Boneworks-Save-Manager/releases/latest | grep tarball_url | awk '{ print $2 }' | sed 's/,$//' | sed 's/"//g' );

# Download latest release of yoyoeditor.tar.gz
release_tar_url=$(curl -s https://api.github.com/repos/zoogies/yoyoengine/releases/latest | grep tarball_url | awk '{ print $2 }' | sed 's/,$//' | sed 's/"//g' );
echo $release_tar_url

download_path="/tmp/yoyoeditor.tar.gz"

echo "Downloading yoyoeditor.tar.gz..."
if ! curl -L -o "$download_path" "$release_tar_url"; then
    echo "Error: Failed to download yoyoeditor.tar.gz."
    exit 1
fi

# Unpack yoyoeditor.tar.gz into /opt/zoogies/yoyoengine
install_dir="/opt/zoogies"

# create install_dir if it doesn't exist
if [ ! -d "$install_dir" ]; then
    echo "Creating $install_dir..."
    if ! sudo mkdir -p "$install_dir"; then
        echo "Error: Failed to create $install_dir."
        exit 1
    fi
fi

echo "Unpacking yoyoeditor.tar.gz..."
if ! sudo tar -xf "$download_path" -C "$install_dir"; then
    echo "Error: Failed to unpack yoyoeditor.tar.gz."
    exit 1
fi

# cleanup download_path
if ! rm "$download_path"; then
    echo "Error: Failed to cleanup $download_path."
    exit 1
fi

# Create a .desktop file
echo "Creating desktop entry..."

desktop_entry="[Desktop Entry]
Version=0
Name=yoyoeditor
Comment=The yoyoengine editor
Exec=/opt/zoogies/yoyoengine/yoyoeditor
Icon=/opt/zoogies/yoyoengine/engine_resources/enginelogo.png
Terminal=true
Type=Application
Categories=Development;IDE;"

echo "$desktop_entry" | sudo tee /usr/share/applications/yoyoeditor.desktop > /dev/null

# Update desktop database
sudo update-desktop-database

# Create desktop shortcut
desktop_shortcut="$HOME/Desktop/yoyoeditor.desktop"
echo "Creating desktop shortcut..."

echo "$desktop_entry" > "$desktop_shortcut"
chmod +x "$desktop_shortcut"

# create ~/.local/share/yoyoengine
if [ ! -d "~/.local/share/yoyoengine" ]; then
    echo "Creating ~/.local/share/yoyoengine..."
    if ! sudo mkdir -p "~/.local/share/yoyoengine"; then
        echo "Error: Failed to create ~/.local/share/yoyoengine."
        exit 1
    fi
fi

# create ~/.local/share/yoyoengine/project_cache.yoyo and echo blank template
if [ ! -f "~/.local/share/yoyoengine/project_cache.yoyo" ]; then
    echo "Creating ~/.local/share/yoyoengine/project_cache.yoyo..."
    if ! sudo touch "~/.local/share/yoyoengine/project_cache.yoyo"; then
        echo "Error: Failed to create ~/.local/share/yoyoengine/project_cache.yoyo."
        exit 1
    fi
    echo "{\"projects\":[]}" | sudo tee "~/.local/share/yoyoengine/project_cache.yoyo" > /dev/null
fi

echo "Installation completed successfully."