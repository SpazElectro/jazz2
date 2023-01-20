import os

from pathlib import Path

from pick import pick
from rich import pretty

pretty.install()

options = []

option, index = pick(["Levels", "Mutators"], "Levels or mutators?")

if index == 0:
    options = Path("./levels").glob("*")
else: options = Path("./mutators/").glob("*")

newOptions = []
for x in options:
    newOptions.append(x)
options = newOptions

option, index = pick(options, "Pick a " + ("level" if index == 0 else "mutator"))

option2, index2 = pick(["CD Directory", "Run"], "CD the directory, or run?")

if index2 == 0:
    os.system("start /B start cmd.exe @cmd /k cd " + str(option) + "")
else:
    os.system("start /B start cmd.exe @cmd /k cd " + str(option) + " && run.bat")

os._exit(0)