import os, json
from pathlib import Path
import copy
from tkinter.filedialog import askdirectory
from tkinter.simpledialog import askstring
from tilesetScanner import scanTileset, ScannedTilesetResult

tilesetsParsed = {}

def scanTilesets(folder: str):
    global tilesetsParsed

    print("Scanning tilesets...")

    for file in os.scandir(folder):
        if file.name.endswith(".j2t"):
            data = open(file.path, "rb").read()
            x: ScannedTilesetResult = scanTileset(data)

            if len(x.Warnings) != 0:
                print(f"Warnings found at {file.name}: {x.Warnings}")
            if len(x.Errors) != 0:
                print(f"Errors found at {file.name}: {x.Errors}")
                continue
            modifiedX = x.to_dict()
            modifiedX["fileName"] = file.name
            tilesetsParsed[x.CRC32] = modifiedX

    print("Finished scanning tilesets!")

    with open("cache2.json", "w") as f:
        f.write(json.dumps(tilesetsParsed))

if Path("cache2.json").exists():
    tilesetsParsed = json.loads(open("cache2.json").read())
else:
    # we could read it from the registry though instead of asking
    directory = askdirectory(title="Select the JJ2 folder")
    if not directory:
        print("No directory was picked!")
        exit(1)
    print(f"Scanning \"{directory}\"...")
    scanTilesets(f"{directory}/cache/")
    scanTilesets(f"{directory}/")

title = input("Title? ").lower()
for tileset in tilesetsParsed:
    if title in tilesetsParsed[tileset]["Title"].lower():
        print(f"{tilesetsParsed[tileset]['fileName']}: {tilesetsParsed[tileset]['Title']}")
