# J2A-extract
Jazz Jackrabbit 2 Anims.j2a file extractor and importer for Python 2 and 3

Created by Stijn and VioletCLM; refactoring and OO interface by Marut.
Compatible with Python 2.6+ and Python 3.0+

## Usage

To extract data from a J2A file:

> python j2a-extract.py <anims_file>

the output folder will be in the same folder as the .J2A file, with the dot swapped for a dash;
for instance if you call this with `C:\Games\Jazz2\Anims.j2a`, the output will be `C:\Games\Jazz2\Anims-j2a\`.

To generate a new J2A file from unpacked data:

> python j2a-import.py <path_to_anims_folder> <new_anims_file>
