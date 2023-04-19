from PIL import Image

# Open the JASC-PAL file
with open('palette.jpal', 'r') as f:
    # Read the header and palette data
    header = f.readline()
    palette_data = f.read()

# Parse the header
if header.strip() != 'JASC-PAL':
    raise ValueError('Invalid JASC-PAL file')

img = Image.new('P', (33, 10))

# Parse the palette data
i = 1
y = 1

for line in palette_data.strip().split('\n'):
    if line == "0100" or line == "256": continue
    r, g, b = line.split()
    print(i)

    if i % 32 == 0:
        y += 1
        i = 0
    img.putpixel((i, y), (int(r), int(g), int(b)))

    i += 1

# Create the image
img.save("yay.png")
