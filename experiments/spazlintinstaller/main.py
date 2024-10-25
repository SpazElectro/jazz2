import os
import shutil

from tkinter import filedialog
from pathlib import Path
from pick import pick
from pyunpack import Archive  # needs `patool`
import winreg

def setenv(name, value):
    key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, "Environment", 0, winreg.KEY_WRITE | winreg.KEY_WOW64_64KEY)
    winreg.SetValueEx(key, name, 0, winreg.REG_EXPAND_SZ, value)
    winreg.CloseKey(key)

def delenv(name):
    key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, "Environment", 0, winreg.KEY_WRITE | winreg.KEY_WOW64_64KEY)
    winreg.DeleteValue(key, name)
    winreg.CloseKey(key)

def getenv(name):
    try:
        key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, "Environment", 0, winreg.KEY_READ | winreg.KEY_WOW64_64KEY)
    
        value, _ = winreg.QueryValueEx(key, name)
    except FileNotFoundError:
        return None
    
    return value

pickOption = -1
gameDir = ""

if getenv("SPAZLINT_DIR") != None:
    installDir = Path(getenv("SPAZLINT_DIR")) # type: ignore
    _, pickOption = pick(
        ["Update spazlint", "Uninstall spazlint", "Cancel"],
        f"Spazlint is already installed at {os.path.abspath(installDir)}",
    )
    if pickOption == 1:
        if installDir.exists():
            shutil.rmtree(installDir)
        else:
            print("SPAZLINT_DIR is defined, but leads to a nonexistent location, removing env variable...")

        delenv("SPAZLINT_DIR")
        print("Spazlint has been successfully uninstalled!")
        exit(1)
    if pickOption == 2:
        print("Have a good day!")
        exit(1)

if pickOption != 0:
    installDir = filedialog.askdirectory(title="Pick a directory to install Spazlint to")

    if installDir == "":
        print("Operation cancelled!")
        exit(1)

    installDir = Path(installDir + "/spazlint")
    installDir.mkdir(exist_ok=True)

    gameDir = filedialog.askopenfilename(title="Select the game exe")
    gameDir = Path(gameDir) # type: ignore

    if gameDir == "":
        print("Operation cancelled!")
        exit(1)
else:
    installDir = Path(getenv("SPAZLINT_DIR")) # type: ignore

    if installDir.exists():
        shutil.rmtree(installDir)
    else:
        print("SPAZLINT_DIR is defined, but leads to a nonexistent location, removing env variable...")
        delenv("SPAZLINT_DIR")
    
    installDir.mkdir(exist_ok=True)

import httpx
import progressbar
import os

url = "http://localhost:8080/spazlint.7z"
filename = "spazlint.7z"

print("Downloading spazlint.7z...")

with httpx.stream("GET", url) as response:
    total_size = int(response.headers.get("Content-Length", 0))

    bar = progressbar.ProgressBar(max_value=total_size, widgets=[
        progressbar.Bar('=', '[', ']'), ' ',
        progressbar.Percentage(), ' ',
        progressbar.SimpleProgress(format='%(value_s)s/%(max_value_s)s bytes'),
        progressbar.FileTransferSpeed(), ' ', progressbar.Timer()
    ])

    progress = 0

    with open(filename, 'wb') as f:
        for chunk in response.iter_bytes():
            f.write(chunk)
            progress += len(chunk)
            bar.update(progress)

    bar.finish()

print("Installing spazlint...")

zip_ref = Archive(filename)
zip_ref.extractall(installDir) # type: ignore

if pickOption == -1:
    open(os.path.abspath(installDir) + "/config.ini", "w").write(f"GAME_DIRECTORY=" + "\\".join(os.path.abspath(gameDir).split("\\")[:-1:]) + "\\" + f"\nGAME_NAME={gameDir}")
os.remove(filename)

setenv("SPAZLINT_DIR", os.path.abspath(installDir))

print(f"Spazlint has been {'installed' if pickOption != 0 else 'upgraded'} successfully to {os.path.abspath(installDir)}!")
