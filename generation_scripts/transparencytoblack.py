from PIL import Image
import os

# Open the palette image and extract its color palette
palette_image = Image.open("yay.png")
palette = palette_image.getpalette()

# Define the color matrix
matrix = (
    0.2126, 0.7152, 0.0722, 0,
    0.2126, 0.7152, 0.0722, 0,
    0.2126, 0.7152, 0.0722, 0,
    0, 0, 0, 1
)

# Iterate over all files in the current directory
for filename in os.listdir('.'):
    # Check if the file is an image
    if filename.endswith('.png') or filename.endswith('.jpg'):
        # Open the image
        image = Image.open(filename)
        
        # Convert the image to RGBA mode
        image = image.convert('RGBA')

        # Replace every transparent pixel with BLACK 0 0 0
        pixels = image.load()
        for x in range(image.width):
            for y in range(image.height):
                if pixels[x, y][3] == 0:
                    pixels[x, y] = (0, 0, 0, 255)

        # Convert the image back to 8-bit color using the yay.png palette and color matrix
        image = image.convert('P', palette=palette, colors=256, matrix=matrix)

        # Save the modified image
        image.save(filename)
