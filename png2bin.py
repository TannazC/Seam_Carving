'''
PNG <-> BIN Image Conversion Module
Author: Tannaz Chowdhury
GitHub: TannazC
Date: 2025

This script defines utility functions to convert standard RGB PNG images into
custom .bin format and back. The binary format stores image dimensions followed
by RGB pixel data row-wise. It also includes a testing block to demonstrate
functionality using sample files.
'''

from PIL import Image  # Import PIL for image reading and manipulation

# ----------------------------------------
# Function: write_image
# ----------------------------------------
'''
Writes a PIL image to a binary file in the following format:
[2 bytes height][2 bytes width][3 * height * width RGB values row-wise]
'''
def write_image(image, filename):
    height = image.height                          # Get image height
    width = image.width                            # Get image width

    f = open(filename, "wb")                        # Open file in binary write mode

    f.write(height.to_bytes(2, byteorder='big'))   # Write height as 2-byte big-endian
    f.write(width.to_bytes(2, byteorder='big'))    # Write width as 2-byte big-endian

    img_raster = []                                # Prepare list for pixel data
    for i in range(height):                        # For every row
        for j in range(width):                     # For every column
            img_raster.extend(image.getpixel((j, i))[:3])  # Append RGB values

    f.write(bytearray(img_raster))                 # Write all RGB bytes at once
    f.close()                                      # Close the file

# ----------------------------------------
# Function: read_2bytes
# ----------------------------------------
'''
Reads two bytes from a binary file and returns the integer.
Used to decode image dimensions stored at the top of .bin files.
'''
def read_2bytes(f):
    bytes = f.read(2)                              # Read two bytes
    return int.from_bytes(bytes, byteorder='big')  # Convert to integer

# ----------------------------------------
# Function: read_image
# ----------------------------------------
'''
Reads a .bin file and reconstructs a PIL RGB image.
Assumes format: [2 bytes height][2 bytes width][RGB RGB RGB ...]
'''
def read_image(filename):
    f = open(filename, "rb")                       # Open file in binary read mode
    height = read_2bytes(f)                        # Read height
    width = read_2bytes(f)                         # Read width

    image = Image.new("RGB", (width, height))      # Create empty image with given dimensions
    bytes = f.read()                               # Read remaining pixel data

    for i in range(height):                        # Loop through each row
        for j in range(width):                     # Loop through each column
            image.putpixel((j, i), (               # Set pixel value at (j, i)
                bytes[3*(i*width + j) + 0],        # Red
                bytes[3*(i*width + j) + 1],        # Green
                bytes[3*(i*width + j) + 2]         # Blue
            ))

    return image                                   # Return reconstructed image

# ----------------------------------------
# Main (test and conversion block)
# ----------------------------------------
if __name__ == '__main__':
    # Convert example PNGs to binary
    image = Image.open("6x5.png")
    write_image(image, "6x5.bin")
    image = Image.open("3x4.png")
    write_image(image, "3x4.bin")

    # Read binary files and save as PNG
    im2 = read_image("a.bin")                      # Read and display (if desired)
    im3 = read_image("6x5_grad.bin")              # Read gradient image
    im3.save("grad.png")                           # Save gradient image as PNG

    # Convert multiple binary image outputs back to PNGs
    for i in range(200):
        image = read_image("img%d.bin" % i)        # Read each binary file
        image.save("img%d.png" % i)                # Save as PNG
