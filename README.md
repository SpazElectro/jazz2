# jazz2
this is where i work on mutators, levels, scripts, etc

you can check my jazz2online profile [here](https://www.jazz2online.com/users/22073/spaz-electro)

## setup
most important thing: `Visual Studio ***Code***`

### building the snippets/autocomplete
go to `generation_scripts`, open `snippets.py` (python 3)
copy the outputted file `snippets_build.json` and go to the `.vscode` folder
paste it and name it `plus.code-snippets` (yes, remove the json and make it `code-snippets`)
and you're done!

### building the icons
go to `generation_scripts`, go to `iconbuilder`, open `build.py` (python 3)
go inside `j2a` and you'll find the built anims file named `icons.j2a`

### good syntax highlight
download `C/C++ Extension Pack` for Visual Studio ****Code****

### notes
a palette has a max of 256 colors, ALL icons should use the default JJ2 palette, My friend [Cranky](https://gitlab.com/Cranky94) has helped me with icons and provided me with a [JASC PaintShop Pro 8 Palette](https://github.com/SpazElectro/jazz2/blob/master/sprites/Jazz%202%20Palette.PspPalette)
important stuff like: hearts and blaster icons are in ANIM::PICKUPS
the blaster icon is different if it's upgraded, if it's upgraded it uses ANIM::AMMO id 19 JAZZ-20 SPAZ, if it's not upgraded it uses ANIM::PICKUPS id 30 JAZZ-31 SPAZ
when trying to use these ids, always subtract one from them because animations are zerobased unlike JazzSD
