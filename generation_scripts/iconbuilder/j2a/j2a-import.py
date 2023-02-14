readme = """
Create a J2A file from a folder of animation sets

The folder should contain one subfolder per animation set. This set folder 
should contain one folder per animation. The animation folder should contain
animation frames as paletted .png files, as well as a Yaml file with the
.settings extension that defines animation properties. In other words, the 
folder you point this script as should have the following structure:

    my_animations ->
        enemies
            -> evil_ghost
                -> frame001.png
                -> frame002.png
                -> evil_ghost.settings
            -> machinegun_marine
                -> ...
        pickups
            -> ...

And so on. The .settings Yaml file should be structured as follows:

    default:
        origin: 16,16
        coldspot: 8,8
        gunspot: 9,4
        tagged=1
        fps=12

    frame005:
        coldspot: 4,3
        tagged=0

And so on. The 'default' dictionary is required, the rest is optional and can
overwrite the settings for individual frames. The dictionary keys should
correspond to frame file names: in the example above, the coldspot and tagged
setting are overridden for frame005.png. Overriding 'fps' for individual frames
has no effect.

Sets, animations & frames are imported in file name order (i.e. 
alphabetically). File and folder names can be arbitrary.
"""

import argparse
import pathlib
import yaml

from PIL import Image
from j2a import J2A

cli = argparse.ArgumentParser(description=readme, prog="J2A Creator", formatter_class=argparse.RawDescriptionHelpFormatter)
cli.add_argument("folder", help="Folder containing animation frames, one folder per animation")
cli.add_argument("--output", help="Output file name", default="?")
cli.add_argument("--yes", "-y", help="Always answer confirmation prompts with 'yes'", default=False,
                 action="store_true")
args = cli.parse_args()

# check and open all relevant files and folders
source_folder = pathlib.Path(args.folder)
if not source_folder.exists():
    print("Folder %s does not exist." % args.folder)
    exit(1)

if args.output == "?":
    args.output = ".".join(args.folder.rsplit("-", 1))
output_file = pathlib.Path(args.output)

set_folders = sorted([subfolder for subfolder in source_folder.iterdir() if subfolder.is_dir()])
j2a_file = J2A(str(output_file), empty_set="crop")
j2a_file.sets = [J2A.Set() for _ in range(len(set_folders))]

# loop through everything and store it
set_index = 0
for set_folder in set_folders:
    animation_folders = sorted([subfolder for subfolder in set_folder.iterdir() if subfolder.is_dir()])
    animation_set = j2a_file.sets[set_index]
    animation_set.animations = [J2A.Animation() for _ in range(len(animation_folders))]
    animation_index = 0

    print("Importing set %s" % set_folder.name)

    for animation_folder in animation_folders:
        frame_files = sorted(animation_folder.glob("*.png"))
        if not frame_files:
            print("No frames found for animation '%s', skipping" % animation_folder.name)
            continue

        settings_file = list(animation_folder.glob("*.settings"))
        if len(settings_file) != 1:
            print("Need exactly one *.fps file for animation %s, %i found, skipping" % (
                animation_folder.name, len(settings_file)))
            continue

        settings_file = settings_file[0]
        with settings_file.open() as settings_input:
            settings = yaml.load(settings_input, Loader=yaml.Loader)

        # require at least a 'default' dictionary with all keys, if not we
        # can't properly store the data
        required_settings = {"origin", "coldspot", "gunspot", "tagged", "fps"}
        if "default" not in settings or type(settings["default"]) != dict or set(
                settings["default"].keys()) & required_settings != required_settings:
            print(
                "Settings file for animation %s does not define all required properties, skipping" % animation_folder.name)
            continue

        print("Importing animation '%s'" % animation_folder.name)
        animation = animation_set.animations[animation_index]
        animation.fps = settings["default"]["fps"]  # cannot be set per-frame

        for frame_num, frame_file in enumerate(frame_files):
            image = Image.open(frame_file)
            if image.mode != "P":
                print("Frame image %s for animation %s is not paletted, skipping" % (
                    frame_file.name, animation_folder.name))
                continue

            # read the settings from default, unless they have been defined for
            # this frame explicitly
            frame_settings = settings.get(frame_file.stem, settings["default"])
            frame = J2A.Frame(
                pixmap=image,
                origin=tuple([int(x) for x in frame_settings.get("origin", settings["default"]["origin"]).split(",")]),
                coldspot=tuple([int(x) for x in frame_settings.get("coldspot", settings["default"]["coldspot"]).split(",")]),
                gunspot=tuple([int(x) for x in frame_settings.get("gunspot", settings["default"]["gunspot"]).split(",")]),
                tagged=bool(frame_settings.get("tagged", settings["default"]["tagged"]))
            )
            frame.autogenerate_mask()

            animation.frames.append(frame)

        animation_index += 1

    animation_set.samplesbaseindex = 0
    animation_set.pack(j2a_file.config)
    set_index += 1

j2a_file.write()
print("Done!")
