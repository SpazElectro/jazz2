from PIL import Image
import os

# iterate over all files in the current directory
for filename in os.listdir('.'):
    # check if the file is an image
    if filename.endswith('.png') or filename.endswith('.jpg'):
        # open the image
        image = Image.open(filename)
        
        # convert the image to RGBA mode
        image = image.convert('RGBA')

        # replace every transparent pixel with BLACK 0 0 0
        pixels = image.load()
        for x in range(image.width):
            for y in range(image.height):
                if pixels[x, y][3] == 0:
                    pixels[x, y] = (0, 0, 0, 255)

        # convert the image back to its original mode
        image = image.convert('P')
        
        # save the modified image
        image.save(filename)