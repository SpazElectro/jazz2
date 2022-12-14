,-------------------------,
|                         |
|  The MUST READ section  |
|                         |
`-------------------------'

It is very important that you back up your Anims.j2a before proceeding. The sprite editor uses an auto-save function whenever any changes are made. Furthermore, once the editor writes to the Anims.j2a file, the changes are irreversible. For safety reasons, read-only mode is enabled until the user manually disables it. On the other hand, it is only recommended to back up the Jazz2.exe file. This is because the patcher can patch and unpatch it, leaving behind the exact original file. The patcher patches one byte in the Jazz2.exe file that tells it to ignore filesize and CRC checks.

Now that you're done with this section, I strongly recommend that you read the rest of the readme.

***************************************

Jazz Sprite Dynamite (JSD) - A working Jazz2 sprite editor

,--------------,
|              |
|  Change Log  |
|              |
`--------------'

v1.01
=====
Fixed: The "File --> Exit" function now works. (But who uses it?)
Added: Ability to import and export to/from test.png
Added: Ability to resize frame
Added: A save function (autosave still exists though)

v1.00
=====
The initial release that revolutionised the Jazz community.

,---------------------------,
|                           |
|  Meaning behind the name  |
|                           |
`---------------------------'

Everytime I used the editor, my graphics always came out MUCH worse than the original, no matter how much time I spent. In other words, my mods always turned out to be a wrecked form of the original, thus dynamite. Also, since there is no undo or undestruction functions (yet), any damage done is permanent. Nevertheless, I do hope that people find good use for the editor and create some kickass mods. In a later version, mods can be shared through some kind of patch (refer to "future" section).

,----------------,
|                |
|  Requirements  |
|                |
`----------------'

* Requires zlib.dll for compression and decompression (included).
* Requires vbpng1.dll for reading from and writing to test.png.
* Requires Visual Basic 6 Runtime Files (msvbvm60.dll)
* Has only been tested on Windows XP so far. Other than that, it should work on any system that can run JJ2.

,-----------------,
|                 |
|  How to use it  |
|                 |
`-----------------'

* If you want to make any changes to anims.j2a, you must disable read-only protection.
* If you want Jazz2 to read the new anims.j2a, you must use the Jazz2 Patcher.

Quickie Guide
=============
(1) Read the MUST READ section
(2) Create backup of anims.j2a
(3) Copy exe and zlib.dll into Jazz2 folder
(4) Start the editor
(5) If the editor cannot start, read the "Requirements" section

Using the editor for exporting GIFs
===================================
(1) Browse through Set, Anim and Frame
(2) Go to "File --> Export to GIF" (Ctrl + E)
(3) AnimXXXX.gif will be saved in the working directory

Using the editor to edit sprites
================================
(1) Browse through Set, Anim and Frame
(2) Choose a color from the palette
(3) Choose a tool from among 6 current tools 

Using the Jazz2 Patcher
=======================
(1) Go to "Tools --> Jazz2 Patcher" (Ctrl + P)
(2) Enter directory + filename (should auto-detect)
(3) Click refresh if auto-detect failed
(4) Click patch (or unpatch) to (un)patch Jazz2.exe

Exporting PNG
=============
(1) Go to "File --> Export frame to test.png"
(2) This will save the current frame as a 24-bit test.png.

Exporting PNG
=============
(1) Go to "File --> Import frame from test.png"
(2) This will load the current frame from test.png.
(3) Take note of three things:
	-> Transparency is compared against pixels that match the background color.
		-> This means that if your background is yellow, your test.png must
		-> use a yellow background for it to be identified as "transparent".
		-> Tip: Change background to white, and you can use white-bg images.
	-> You can use any colors in test.png, its automatically remapped onto the palette.
	-> The editor will not read test.png if it is a not a 24-bit PNG file.

,-----------------------,
|                       |
|  Parts of the editor  |
|                       |
`-----------------------'

On the top left-hand corner we have:
====================================
* Set ID (scrollbar and textbox)
* Animation (scrollbar and textbox)
* Frame # (scrollbar and textbox)
* Animation controls
    -> Loop (plays animation in a continuous loop)
    -> Ping-pong (plays animation back and forth)
    -> Show thumbnail (shows a thumbnail of working frame)
    -> Play animation once (Plays the animation once)

On the top right-hand corner we have:
=====================================
* Animation display box
    -> This shows the currently selected animation

On the bottom left-hand corner we have:
=======================================
* Frame edit box (very much like MSPaint interface)
    -> This shows the currently selected frame
    -> Is affected by zoom and appearance settings
    -> Can edit the sprite via tools, or menu functions

On the bottom right-hand corner we have:
========================================
* Toolbox
    -> Pencil (sets individual pixels)
    -> Line (draws a line)
    -> Rect (draws a rectangle, shift to toggle filled/unfilled)
    -> Circle (draws an ellipse, shift to toggle filled/unfilled)
    -> Flood Fill (fills a region of the same color)
    -> Color Picker (picks a color on the frame edit box)
    -> Current Color (shows the currently selected color)
* Zoom Settings
    -> No Zoom (1x)
    -> 2-10x
    -> Fit Width (automatically adjusts zoom to fit width)	
    -> Fit Height (automatically adjusts zoom to fit height)
    -> Overall Fit (maximum zoom such that scrollbars are not required)
* Appearance Settings
    -> Checkboxes (toggle) and color (doubleclick) for each property:
          Grid, Hotspot, Coldspot, Gunspot (Boundary can only be set in INI)
    -> Background (sets the background color of frame and animation box)
* Palette Box
    -> Automatically updates when changing sets
    -> Represents the colors in the current palette
    -> Can be used to select the current color
* Statistics Box
    -> Everything should be straightforward

,------------------,
|                  |
|  Advanced Stuff  |
|                  |
`------------------'

Terminology
===========
Hotspot = The point where the sprite is rendered
Coldspot = Not completely figured out, something to do with gravity
Gunspot = The starting point where the bullet/bomb/stuff is rendered
Boundary = The smallest rectangle that can fit the image in the frame

Menu
====
File
  -> Make file Read-only (toggles readonly)
  -> Export to GIF (saves animation to AnimXXXX.gif)
  -> Import GIF (incomplete code)
  -> Export frame to test.png (saves 24-bit test.png)
  -> Import frame from test.png (reads 24-bit test.png)
  -> Save (saves to anims.j2a)
  -> Exit (exits program)
Set
  -> Nothing to see here
Animation
  -> Resize Frame (changes the size of the frame)
  -> Set FPS (sets FPS, but read warning first)
  -> Another set of incomplete functions
  -> Clear Frame (clears frame, but read warning first)
  -> Clone onto Next Frame (copies current frame onto next frame, then loads next frame)
  -> Reverse animation (Frame 1 becomes last frame, etc)
  -> Flip Frame Vertically
Tools
  -> Jazz2 Patcher (brings up the patcher subprogram)
  -> Additional Settings (incomplete code)
Help
  -> About (brief message with information about the program)

Undocumented Hotkeys
====================
QW	- Move the SetID scrollbar left or right
AS	- Move the Animation scrollbar left or right
ZX	- Move the Frame scrollbar left or right
Numpad/ - Decrease palette index
Numpad* - Increase palette index
Numpad+	- Increase zoom setting
Numpad- - Decrease zoom setting

INI Settings
============
Currently too lazy to list all, but the undocumented ones are:
  ShowBoundary (default = 0) - Sets whether boundary should be displayed
  BoundaryColor (default = 255) - Sets the color for the boundary box
  ShiftToFill (default = 1) - Sets whether rect/circle are filled/unfilled when shift is pressed/unpressed
  UseDottedGrid (default = 0) - Sets whether the grid should use dotted instead of solid lines
If you haven't already noticed, colors are all stored as 32-bit RGB ints.

Easter Eggs
===========
If I revealed these then they wouldn't be Easter Eggs, would they? ;)

,------------,
|            |
|  Warnings  |
|            |
`------------'

* Backup Anims.j2a ;)

* The import/export PNG function can only read/write 24-bit PNGs. If your PNG is 8-bit or less, the editor will not read it, so be sure to increase the color depth.

* There is very little error-handling coded in the editor. If any error occurs, the VB library runs the default error-handling procedure, which is an error message, and then a crash. <--- which means you lose all work

* If you play on-line, no one else can see your changed animations.

* If you clear a frame, draw on it instead of going to another animation, then back. Otherwise, the save function automatically resizes the frame, and you might find that you have much less space to work with.

* The autosave function writes to anims.j2a everytime the set is changed, or the program is terminated. If you realise you have made a change to a frame that you do not want to save, you can enable read-only, then change sets and back. Note that by doing this, you lose all other changes you have made to the other animations or frames.

* There is no save function (yet). To permanently embed any changes, change the SetID and back.

* (The FPS value shown in the statistics box does not reflect the real FPS value. The real FPS is a function of this FPS value which I have not attempted to calculate.) <-- I'm not even sure if this is true or not as I haven't tested anything. 

* The editor has only been tested on v1.23 and v1.24 (TSF). While the editor could probably open other versions of Anims.j2a as well, the editor might allocate wrong palette for certain sets. Furthermore, editing the sprites will be useless anyway if the exe remains unpatched (patch only works for v1.23 and v1.24).

,-------------------,
|                   |
|  Rants and raves  |
|                   |
`-------------------'

* Despite being a native UK english speaker, I have used the word 'color' in all places where 'colour' would have been the right spelling. This is to satisfy the general population (I assume most of the Jazzers are American?), and also because it has one less letter to type.

* Even though MSDN keeps saying that modern programs should always use the registry, I still like the INI. Firstly because it is easier to edit, and secondly because you can have different versions for different instances of the exe.

* I first started this project using Visual C++. The first implementation was a simple command line, which uncompressed the wanted sprite, edit it (e.g. using MSPaint) then compress it back into the anims.j2a. I wanted to make a GUI, but I realised I was too lazy to code a proper GUI so I decided to use Visual Basic instead. In the end I used mostly APIs for my VB window procedures so it ended up the same thing. In other words, if I feel like it I will re-do the whole thing in C++, which will only require simple conversion. C++ will be faster and more "proper".

* Furthermore, my VB code has attained the awesome status of UNMAINTAINABLE, so all the more reason to re-create the whole thing in C++. The code is officially defined as unmaintainable by the popular document "How to Write Unmaintainable Code".

* Drawing to GIF currently uses an extremely slow method, it converts the palette to RGB, then back to palette. It is my first time writing to GIFs in any way, so I will update it as I learn more. Also, the GIF, despite using LZW (illegal?), is not maximum-compressed. There is also only one GIF palette at the moment. (meaning the menu sprites will come out all wrong)

* I had initially planned to implement the "Set xxSpot" functions, but realise that this can potentially cause cheating. I will not discuss how such a function can be exploited, but by not implementing it, the editor is no longer a cheating tool.

* The libpng, while an amazing library for C++, is quite hard to use in VB. The closest thing I could find was vbpng1, which is an older version compiled especially for VB. Still, it is extremely limited in terms of usage. This is largely due to the way it handles files and arrays, all C++ style. As a result, I had to use C++ wrappers in VB.

,---------------------,
|                     |
|  Credits/Copyright  |
|                     |
`---------------------'

(C)2006 Neobeo, All rights reserved.
This project is released as freeware, which means that the source code is NOT open to the public. However, you can request for the source code if you need it for a particular purpose. Note that the code is in a very unmaintainable state as mentioned before, and is also completely uncommented. When a C++ version of the editor is made, it is likely to be an opensource project.

Additional Credits:
zlib (C)1995-2004 Jean-loup Gailly and Mark Adler
GIF Encoder (C)2001 Ron van Tilburg and Carles P.V.
vbpng1 - unknown license

,-------------------------,
|                         |
|  What the future holds  |
|                         |
`-------------------------'

* I probably mentioned this a dozen times, but the code will probably be ported to C++. This shouldn't be too hard because most of my VB code is handled with APIs, even the window procedures.

* Meanwhile, I might not be updating the VB version of the editor. When I port it to C++, I will include updates such as:
    -> Enable currently disabled/incomplete features (set spots, etc)
    -> Exporting individual frames (with real filenames rather than test.png)
    -> Importing animation, with dither
    -> Importing individual frames, with dither
    -> Swap 2 frames
    -> Swap 2 anims
    -> Swap 2 sets
    -> A built-in hex editor to view the binary version of the sprite
    -> Import/Export changes made to anims.j2a as a mod, which can be shared
    -> Very important - an undo buffer (for undoing)
    -> Ability to copy and paste
    -> A sound extractor (included inside anims.j2a)

* The following features have been considered, but might render the anims.j2a unusable
    -> Adding/removing animation in a set
    -> Adding/removing a set