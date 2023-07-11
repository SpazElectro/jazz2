import subprocess
from typing import List
import uuid
import shutil
import pygetwindow
from inireader import getFromRun

# TODO: read from ini
game_directory = getFromRun("GAME_DIRECTORY")
game_exe = f"{game_directory}{getFromRun('GAME_NAME')}"

def getErrors(mutatorLocation):
    mutatorName = f"spazlint_temp-{uuid.uuid4()}.mut"

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
        for w in windows:
            if "Jazz Jackrabbit 2" in w.title or "chatlogger" in w.title:
                jj2Window = w
                w.minimize()
                break
        
        output += line.decode("utf-8")

        if line.decode("utf-8").startswith("ANGELSCRIPT:"):
            if "Please correct the errors in the script and try again" in line.decode("utf-8"):
                if jj2Window:
                    jj2Window.close()
                break
            errors.append(line.decode("utf-8").strip().split(".mut ")[1])

        if "loaded script module" in output or "Please correct the errors in the script and try again." in output:
            if jj2Window:
                jj2Window.close()
            break

    process.stdout.close()
    return_code = process.wait()
    
    return errors

if __name__ == "__main__":
    import colorama

    colorama.init()
    for e in getErrors("../../mutators/zombies/STVzombies.mut"):
        print(colorama.Fore.RED + e + colorama.Fore.RESET)
