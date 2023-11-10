"""
    This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

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

# script created entirely by chatgpt3.5 (im lazy)
# re-uploaded from https://github.com/Yoyolick/SCDG on 11/10/2023

import os
import re
import shutil

def rename_files(folder_path):
    if not os.path.exists(folder_path):
        print("Folder path doesn't exist.")
        return

    pattern = r'frame_(\d+)_delay-\d+\.\d+s\.(\w+)'
    total_frames = 0

    for filename in os.listdir(folder_path):
        old_path = os.path.join(folder_path, filename)
        if os.path.isfile(old_path):
            match = re.match(pattern, filename)
            if match:
                frame_number = int(match.group(1))
                if frame_number == 0:
                    new_frame_number = "0"
                else:
                    new_frame_number = str(frame_number)
                extension = match.group(2)
                new_filename = f"{new_frame_number}.{extension}"
                new_path = os.path.join(folder_path, new_filename)
                shutil.move(old_path, new_path)
                print(f"Renamed '{filename}' to '{new_filename}'")
                total_frames += 1

    print(f"Total number of frames: {total_frames}")

if __name__ == "__main__":
    folder_path = input("Enter the folder path: ")
    rename_files(folder_path)