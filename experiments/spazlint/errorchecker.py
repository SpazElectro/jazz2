import subprocess
from typing import List
import uuid
import shutil
import pygetwindow
from inireader import getFromRun
from pathlib import Path
import time
import win32gui

# TODO: make this not show the window at all somehow
game_directory = getFromRun("GAME_DIRECTORY")
game_exe = f"{game_directory}{getFromRun('GAME_NAME')}"

def getErrors(mutatorLocation):
    # this kind of acts like backups so, we won't delete
    # maybe we can have an option to delete them, though.
    mutatorName = f"spazlint/spazlint_temp-{uuid.uuid4()}.mut"

    Path(game_directory + "/spazlint").mkdir(exist_ok=True)

    shutil.copyfile(mutatorLocation, f"{game_directory}{mutatorName}")

    command = f"\"{game_exe}\" -mutators={mutatorName} -nochatlogger -server -maxcache2 -nosound -noreg -nojoy -noddrawmemcheck -noddraw -nodsound -nospy -nolog -windowed"

    process = subprocess.Popen(
        command,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        shell=True
    )

    output = ""
    errors = []

    for line in iter(process.stdout.readline, b''):
        windows: List[pygetwindow.Win32Window] = pygetwindow.getAllWindows()
        jj2Window = None
        vscWindow = None
        for w in windows:
            if "Visual Studio Code" in w.title:
                vscWindow = w
            if "Jazz Jackrabbit 2" in w.title:
                jj2Window = w
                win32gui.ShowWindow(w._hWnd, 11)
        if win32gui.GetForegroundWindow() == jj2Window._hWnd:
            win32gui.SetForegroundWindow(vscWindow._hWnd)

        output += line.decode("utf-8")

        if "ANGELSCRIPT: Please correct the errors in the script and try again." in output:
            while jj2Window == None:
                time.sleep(1)
            jj2Window.close()
            break

        if line.decode("utf-8").startswith("ANGELSCRIPT:"):
            errors.append(line.decode("utf-8").strip().split(".mut ")[1])

        if "loaded script module" in output:
            while jj2Window == None:
                time.sleep(1)
            jj2Window.close()
            break

    process.stdout.close()
    process.wait()
    
    return errors

if __name__ == "__main__":
    import colorama

    colorama.init()
    for e in getErrors("../../mutators/zombies/STVzombies.mut"):
        print(colorama.Fore.RED + e + colorama.Fore.RESET)
