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

"""
Custom launcher for the editor.
Ryan Zmuda - 2023

Takes the load off of me writing this in C and wasting time.
Uses https://github.com/TomSchimansky/CustomTkinter

TODO:
    - automatically installing an editor version for you
    - deleting projects
    - open existing project and checking its validity
"""

import tkinter as tk
from tkinter import filedialog
import json
import os
import subprocess
from CTkMessagebox import CTkMessagebox
import customtkinter

script_dir = os.path.dirname(os.path.abspath(__file__))
binary_path = "./editor"

# Set appearance mode and default color theme
customtkinter.set_appearance_mode("dark")  # Modes: system (default), light, dark
customtkinter.set_default_color_theme("green")  # Themes: blue (default), dark-blue, green

# Create CTk window
app = customtkinter.CTk()
app.geometry("900x600")
app.title("Yoyo Engine Launcher")
icon = tk.PhotoImage(file=os.path.join(script_dir, "resources/icon.png"))
app.iconphoto(True, icon)

running = False

# Open the settings for the launcher
def set_editor_path():
    global launcher_json
    editor_binary_path = filedialog.askopenfile(title="Select the Editor Binary")
    if editor_binary_path is None:
        return
    editor_binary_path = editor_binary_path.name
    # Update launcher.json if editor binary path is valid
    if os.path.exists(editor_binary_path):
        launcher_json["editor binary"] = editor_binary_path
        update_json()
    else:
        CTkMessagebox(title="Error", message="Your editor path does not exist. Please try again.", icon="cancel")

# Check if launcher.json exists, create it if it doesn't
if not os.path.exists("./launcher.json"):
    with open("./launcher.json", "w") as f:
        f.write("{\n\t\"projects\":[],\n\t\"editor binary\":\"./build/editor\"\n}")

# Load launcher.json into a dict
with open("./launcher.json", "r") as f:
    try:
        launcher_json = json.load(f)
        # check that editor binary path exists, give error if not
        if not os.path.exists(launcher_json["editor binary"]):
            CTkMessagebox(title="Error", message="Your editor path does not exist. Please update it.", icon="cancel")

    except json.JSONDecodeError:
        CTkMessagebox(title="Error", message="An error occurred parsing the launcher data. Please ensure its validity.", icon="cancel")

# Open the editor with the specified project
def open_editor(project_path):
    global running
    if running:
        try:
            subprocess.Popen([launcher_json['editor binary'], project_path])
            app.quit()
        except:
            CTkMessagebox(title="Error", message="Your editor or project path could not be resolved. Please double check both.", icon="cancel")

# Update launcher.json
def update_json():
    with open("./launcher.json", "w") as f:
        json.dump(launcher_json, f, indent=4)

# Create a new project
def new_project():
    global launcher_json
    dialog = customtkinter.CTkInputDialog(text="Enter the name of the project:", title="New Project")
    project_name = dialog.get_input()  # waits for input

    # Use tkinter dialog to select folder for project
    project_path = filedialog.askdirectory(title="Select Project Directory")

    # check that directory does not already exist
    if os.path.exists(os.path.join(project_path, project_name)):
        CTkMessagebox(title="Error", message="A project with that name already exists. Please try again.", icon="cancel")
        return

    # Create a new directory for the project
    new_project_path = os.path.join(project_path, project_name)

    # copy ./templates/* to new_project_path
    import shutil
    print(os.path.join(script_dir, "template"))
    shutil.copytree(os.path.join(script_dir, "template"), new_project_path)

    # Update launcher.json
    launcher_json["projects"].append({"name": project_name, "path": project_path})
    update_json()

    open_editor(new_project_path)

# Create buttons for new project and settings
new_project_button = customtkinter.CTkButton(master=app, text="New Project", command=new_project)
new_project_button.place(relx=.12, rely=.05, anchor=customtkinter.CENTER)

settings_button = customtkinter.CTkButton(master=app, text="Set Editor Path", command=set_editor_path)
settings_button.place(relx=.32, rely=.05, anchor=customtkinter.CENTER)

# prompt for the project name, check if it exists, delete it if it does after prompting are you sure yes/no
def delete_project():
    global launcher_json
    dialog = customtkinter.CTkInputDialog(text="Enter the name of the project to delete:", title="Delete Project")
    project_name = dialog.get_input()  # waits for input

    project_path = None

    # check that project exists
    project_exists = False
    for project in launcher_json["projects"]:
        if project["name"] == project_name:
            project_exists = True
            project_path = project["path"]
            break

    if not project_exists:
        CTkMessagebox(title="Error", message="A project with that name does not exist. Please try again.", icon="cancel")
        return

    # check that user is sure
    msg = CTkMessagebox(title="Delete Project?", message="Are you absolutely sure you want to delete the project at " + project_path + "/" + project_name + "? THIS IS IRRIVERSIBLE!",
                        icon="question", option_1="Cancel", option_2="No", option_3="Yes")
    response = msg.get()

    if response=="Yes":
        # delete project
        for project in launcher_json["projects"]:
            if project["name"] == project_name and project["path"] == project_path:
                launcher_json["projects"].remove(project)
                # os delete the project (platform independent)
                import shutil
                shutil.rmtree(os.path.join(project_path, project_name))
                CTkMessagebox(title="Success", message=project_name+" was deleted.", icon="info")
                update_json()
                # restart the launcher
                app.quit()
                subprocess.Popen(["python3", os.path.join(script_dir, "launcher.py")])
    else:
        CTkMessagebox(title="Aborted", message=project_name+" was not deleted.", icon="info")
    


delete_button = customtkinter.CTkButton(master=app, text="Delete Project", command=delete_project)
delete_button.place(relx=.52, rely=.05, anchor=customtkinter.CENTER)

# Create scrollable frame for projects
scrollable_frame = customtkinter.CTkScrollableFrame(app, width=800, height=500)
scrollable_frame.place(relx=.5, rely=0.53, anchor=customtkinter.CENTER)

# Create a button for each project in launcher.json
for project in launcher_json["projects"]:
    full_path = os.path.join(project["path"], project["name"])
    print(full_path)
    project_button = customtkinter.CTkButton(master=scrollable_frame, text=project["name"], command=lambda project_path=project["path"]: open_editor(full_path), fg_color="gray", bg_color="gray")
    project_button.pack(fill="x", padx=10, pady=10)

# Create label for version
version_label = customtkinter.CTkLabel(master=app, text="Yoyo Engine Launcher v0.0.1")
version_label.place(relx=.5, rely=.98, anchor=customtkinter.CENTER)

running = True

app.mainloop()
