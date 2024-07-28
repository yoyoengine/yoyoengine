#!/bin/bash

# This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
# Copyright (C) 2024  Ryan Zmuda
# Licensed under the MIT license. See LICENSE file in the project root for details.

# Remove installation directory
install_dir="/opt/zoogies/yoyoengine"
if [ -d "$install_dir" ]; then
	echo "Removing $install_dir..."
	if ! sudo rm -rf "$install_dir"; then
		echo "Error: Failed to remove $install_dir."
		exit 1
	fi
else
	echo "$install_dir does not exist."
fi

# Remove desktop entry
desktop_entry="/usr/share/applications/yoyoeditor.desktop"
if [ -f "$desktop_entry" ]; then
	echo "Removing $desktop_entry..."
	if ! sudo rm "$desktop_entry"; then
		echo "Error: Failed to remove $desktop_entry."
		exit 1
	fi
else
	echo "$desktop_entry does not exist."
fi

# Remove desktop shortcut
desktop_shortcut="$HOME/Desktop/yoyoeditor.desktop"
if [ -f "$desktop_shortcut" ]; then
	echo "Removing $desktop_shortcut..."
	if ! rm "$desktop_shortcut"; then
		echo "Error: Failed to remove $desktop_shortcut."
		exit 1
	fi
else
	echo "$desktop_shortcut does not exist."
fi

# remove ~/.local/share/yoyoengine
local_share_dir="$HOME/.local/share/yoyoengine"
if [ -d "$local_share_dir" ]; then
	echo "Removing $local_share_dir..."
	if ! rm -rf "$local_share_dir"; then
		echo "Error: Failed to remove $local_share_dir."
		exit 1
	fi
fi

echo "Uninstallation completed successfully."