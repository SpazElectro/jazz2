# The easiest way(s) to run your JJ2+ scripts!

First steps first, let's start by making your "database", Create a new folder, name it whatever you want, I use/prefer `jazz2`, create 2 new folders, `levels` and `mutators`, now you can follow these simple steps on how to create scripted levels and mutators fast:

* Alright, first off, you'll need [Python 3](https://www.python.org) (and a Windows machine to play test)
* Clone [this repo](https://github.com/SpazElectro/jazz2), using the "advanced way" by doing `git clone https://github.com/SpazElectro/jazz2.git` or just downloading it normally by pressing `Code` and `Download ZIP`
* Open the folder and copy: `create_new.py`,`empty.j2l` and now paste that into your "database"
* Create a new file, called: "run.ini", and make it follow this format, and replace the properties with your own:
```ini
GAME_DIRECTORY=F:\Games\Jazz2\
GAME_NAME=Jazz2+.exe
```
* Run `create_new.py` and make an example mutator, let's call it ~~`babysfirst`~~ `mansfirst`, don't make your (mutator/level)'s name have: spaces, special characters, unicode

Awesome, You've now created a mutator!