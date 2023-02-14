readme = """
Unpack a J2A file

Will unpack a J2A file insto a given folder. The resulting folder structure will
look like this:

    [target folder] ->
        set-001 ->
            animation-001 ->
                frame-001.png
                frame-002.png
                ...
                animation.settings
            animation-002 ->
                ...
        set-002 ->
            ...
        ...

Existing files will be overwritten.
"""

import argparse
import pathlib
import yaml
import os

from j2a import J2A

cli = argparse.ArgumentParser(description=readme, prog="J2A Unpacker", formatter_class=argparse.RawDescriptionHelpFormatter)
cli.add_argument("--palettefile", "-p", default="Diamondus_2.pal", help="Palette file to use")
cli.add_argument("j2afile", help="The J2A file to extract")
cli.add_argument("--folder", "-f", default="?",
                 help="Where to extract the animation data. Defaults to current working directory.")
args = cli.parse_args()

# check if all files we need exist and can be opened properly
if args.folder == "?":
    args.folder = "-".join(args.j2afile.rsplit(".", 1))
destination_folder = pathlib.Path(args.folder)
source_file = pathlib.Path(args.j2afile)
palette_file = pathlib.Path(args.palettefile)

for check_file in (source_file, palette_file):
    if not check_file.exists():
        print("File '%s' does not exist." % str(check_file))
        exit(1)

try:
    j2afile = J2A(str(source_file), palette=str(palette_file)).read()
except Exception:
    print("Could not open J2A file %s. Is it a valid J2A file?" % source_file.name)
    exit(1)

# loop through all animations and unpack their frames
for set_index, set in enumerate(j2afile.sets):
    print("Importing set %i..." % set_index)
    set_folder = destination_folder.joinpath("set-%s" % str(set_index).zfill(3))
    if not set_folder.exists():
        os.makedirs(set_folder)

    for animation_index, animation in enumerate(set.animations):
        print("Unpacking animation %i..." % animation_index)
        animation_folder = set_folder.joinpath("animation-%s" % str(animation_index).zfill(3))
        if not animation_folder.exists():
            os.makedirs(animation_folder)

        settings_file = animation_folder.joinpath("animation.settings")
        settings = {
            "default": {"origin": "0,0", "coldspot": "0,0", "gunspot": "0,0", "tagged": 0, "fps": animation.fps}}

        for frame_index, frame in enumerate(animation.frames):
            frame_file = animation_folder.joinpath("frame-%s.png" % str(frame_index).zfill(3))
            frame_name = frame_file.stem

            settings[frame_name] = {
                "origin": ",".join([str(coordinate) for coordinate in frame.origin]),
                "coldspot": ",".join([str(coordinate) for coordinate in frame.coldspot]),
                "gunspot": ",".join([str(coordinate) for coordinate in frame.gunspot]),
                "tagged": {False: 0, True: 1}[frame.tagged]
            }

            j2afile.render_paletted_pixelmap(frame).save(str(frame_file))

        with settings_file.open("w") as settings_output:
            yaml.dump(settings, settings_output)

print("Done!")
