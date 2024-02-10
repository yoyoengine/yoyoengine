"""
    This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
"""

# script created (almost) entirely by chatgpt3.5 (im lazy)

import os
from PIL import Image
import re
import json

def sort_key(file):
    match = re.search(r'\d+$', file)
    if match:
        return int(match.group())
    else:
        return file

def create_animation_metadata(folder_path):
    frame_files = sorted(os.listdir(folder_path), key=sort_key)

    if not frame_files:
        print("No frame files found in the specified folder.")
        return

    # Create a new image by stacking frames vertically
    frames = [Image.open(os.path.join(folder_path, file)) for file in frame_files if not file.endswith('.aseprite')]
    width, height = frames[0].size
    stacked_image = Image.new("RGBA", (width, height * len(frames)))

    for i, frame in enumerate(frames):
        stacked_image.paste(frame, (0, i * height))

    animation_name = input("Enter animation name: ")

    # Save the stacked image
    # stacked_image.save(os.path.join(folder_path, f"{animation_name}.png"))

    # Get other metadata from user input or individual frame metadata
    version = 0
    frame_width = width
    frame_count = len(frames)
    frame_height = height / frame_count
    frame_delay = int(input("Enter frame delay in milliseconds: "))
    loops = int(input("Enter number of loops (-1 for infinite): "))

    # Create metadata dictionary
    metadata = {
        "name": animation_name,
        "version": version,
        "src": f"animations/{animation_name}/{animation_name}.png",
        "frame_width": int(frame_width),
        "frame_height": int(frame_height),
        "frame_count": int(frame_count),
        "frame_delay": int(frame_delay),
        "loops": int(loops)
    }

    return metadata, stacked_image

folder_path = input("Path to folder containing frame files: ")
meta, image = create_animation_metadata(folder_path)

# ask the user for a resources path
resources_path = input("Enter the path to the resources folder: ")

# ask the user where they want the metadata file to be saved
metadata_path = input("Enter the resources path to save the metadata file: ")

# copy the metadata to the metadata_path and save "meta" as <name>.yoyo in the <resourcesfolder>/animations
metapathagain = os.path.join(resources_path, metadata_path, f"{meta['name']}.yoyo")
with open(metapathagain, "w") as f:
    json.dump(meta, f)

# save the stacked image to the resources folder
image.save(os.path.join(resources_path, f"animations/{meta['name']}", f"{meta['name']}.png"))