'''
PNG to BIN Image Converter and Verifier
Author: Prof. Micheal Guerzhoy 
GitHub: TannazC
Date: 2025

This script converts a PNG image to a binary (.bin) format suitable for use
with the seam carving C implementation. It then reads the binary file
back and displays both images to visually confirm correctness.
'''

import png2bin               # Import custom module to handle binary image format
from PIL import Image        # PIL is used for reading and displaying standard image files

# Run this block only when the script is executed directly
if __name__ == '__main__':
    im2 = Image.open("HJoceanSmall.png")        # Open the PNG image to be converted
    im2.show()                                  # Display the original PNG image

    png2bin.write_image(im2, "HJoceanSmall.bin") # Convert PNG to .bin format for use in seam carving C program

    im = png2bin.read_image("HJoceanSmall.bin") # Read back the binary image to check for conversion integrity
    im.show()                                   # Display the image read from the .bin file to visually confirm match
