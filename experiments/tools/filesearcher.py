import os, json
from pathlib import Path
from levelScanner import scanLevel, ScannedLevelResult

folder = "F:/Games/Jazz2/cache/"

levelsParsed = {}

def scanLevels():
    global levelsParsed
    if Path("cache.json").exists():
        levelsParsed = json.loads(open("cache.json").read())
        return

    print("Scanning levels...")

    # this scans everything to make our life easier
    for file in os.scandir(folder):
        if file.name.endswith(".j2l"):
            data = open(file.path, "rb").read()
            x: ScannedLevelResult = scanLevel(data)

            if len(x.Warnings) != 0:
                print(f"Warnings found at {file.name}: {x.Warnings}")
            if len(x.Errors) != 0:
                print(f"Errors found at {file.name}: {x.Errors}")
                continue
            modifiedX = x.copy()
            modifiedX["fileName"] = file.name
            levelsParsed[x.LevelName] = modifiedX

    print("Finished scanning levels!")

    with open("cache.json", "w") as f:
        f.write(json.dumps(levelsParsed))

scanLevels()

import tkinter as tk
from tkinter import ttk
filename_entry = None
levelname_entry = None
tileset_entry = None
helpstring_entry = None

def filter_treeview():
    keyword_filename = filename_entry.get().lower()
    keyword_levelname = levelname_entry.get().lower()
    keyword_tileset = tileset_entry.get().lower()
    keyword_helpstring = helpstring_entry.get().lower()

    levelList.delete(*levelList.get_children())
    for i, level in enumerate(levelsParsed):
        lvl: ScannedLevelResult = ScannedLevelResult.from_dict(levelsParsed[level])
        if (keyword_filename in lvl["fileName"].lower()
                and keyword_levelname in lvl.LevelName.lower()
                and keyword_tileset in lvl.TileSetName.lower()
                and any(keyword_helpstring in help_string.lower() for help_string in lvl.HelpStrings)):
            levelList.insert('', 'end', text=i, values=(
                i,
                lvl["fileName"], lvl.LevelName, lvl.PasswordProtected, lvl.HideLevel, lvl.TileSetName,
                lvl.HelpStrings[0], lvl.HelpStrings[1], lvl.HelpStrings[2], lvl.HelpStrings[3],
                lvl.HelpStrings[4], lvl.HelpStrings[5], lvl.HelpStrings[6], lvl.HelpStrings[7],
                lvl.HelpStrings[8], lvl.HelpStrings[9], lvl.HelpStrings[10], lvl.HelpStrings[11],
                lvl.HelpStrings[12], lvl.HelpStrings[13], lvl.HelpStrings[14], lvl.HelpStrings[15],
            ))

def open_filter_window():
    global filename_entry, levelname_entry, tileset_entry, helpstring_entry
    filter_window = tk.Toplevel(window)
    filter_window.title("Filter Options")

    filename_label = ttk.Label(filter_window, text="Level Filename:")
    filename_label.grid(row=0, column=0, padx=5, pady=5)
    filename_entry = ttk.Entry(filter_window)
    filename_entry.grid(row=0, column=1, padx=5, pady=5)

    levelname_label = ttk.Label(filter_window, text="Level Name:")
    levelname_label.grid(row=1, column=0, padx=5, pady=5)
    levelname_entry = ttk.Entry(filter_window)
    levelname_entry.grid(row=1, column=1, padx=5, pady=5)

    tileset_label = ttk.Label(filter_window, text="Tileset Name:")
    tileset_label.grid(row=2, column=0, padx=5, pady=5)
    tileset_entry = ttk.Entry(filter_window)
    tileset_entry.grid(row=2, column=1, padx=5, pady=5)

    helpstring_label = ttk.Label(filter_window, text="Help String:")
    helpstring_label.grid(row=3, column=0, padx=5, pady=5)
    helpstring_entry = ttk.Entry(filter_window)
    helpstring_entry.grid(row=3, column=1, padx=5, pady=5)

    filter_button = ttk.Button(filter_window, text="Apply Filters", command=filter_treeview)
    filter_button.grid(row=4, column=0, columnspan=2, padx=5, pady=5)

window = tk.Tk()
window.geometry("800x600")

# Create a Frame to hold the treeview and scrollbars
frame = tk.Frame(window)
frame.pack(fill="both", expand=True)

# Create vertical scrollbar
vsb = ttk.Scrollbar(frame, orient="vertical")
vsb.pack(side="right", fill="y")

# Create horizontal scrollbar
hsb = ttk.Scrollbar(frame, orient="horizontal")
hsb.pack(side="bottom", fill="x")

# Create the treeview and configure it to use the scrollbars
columns = [f"c{i}" for i in range(6 + 16)]
levelList = ttk.Treeview(frame, column=columns, show="headings", height=300,
                         yscrollcommand=vsb.set, xscrollcommand=hsb.set)
levelList.pack(side="left", fill="both", expand=True)

vsb.config(command=levelList.yview)
hsb.config(command=levelList.xview)

levelList.column("# 1", anchor=tk.CENTER)
levelList.heading("# 1", text="Id")
levelList.column("# 2", anchor=tk.CENTER)
levelList.heading("# 2", text="Level filename")
levelList.column("# 3", anchor=tk.CENTER)
levelList.heading("# 3", text="Level name")
levelList.column("# 4", anchor=tk.CENTER)
levelList.heading("# 4", text="Is password protected")
levelList.column("# 5", anchor=tk.CENTER)
levelList.heading("# 5", text="Level hidden in singleplayer menu")
levelList.column("# 6", anchor=tk.CENTER)
levelList.heading("# 6", text="Tileset filename")

for i in range(7, 16 + 7):
    levelList.column(f"# {i}", anchor=tk.CENTER)
    levelList.heading(f"# {i}", text=f"Help string {i-7}")

open_filter_window()

window.mainloop()